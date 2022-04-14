/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <aes.h>
#include <cryptlib.h>
#include <files.h>
#include <filters.h>
#include <gcm.h>
#include <osrng.h>
#include <scrypt.h>

#include "encryptor.h"

// The encryption algorithm was adapted from the AES-GCM-Test.zip file from
// https://www.cryptopp.com/wiki/Advanced_Encryption_Standard#Downloads

// The hashing algorithm was adopted from https://www.cryptopp.com/wiki/Scrypt#OpenMP

/*
 * Layout of encrypted string:
 *
 * [SALT_SIZE bytes for the salt]
 * [IV_SIZE bytes for the IV]
 * [... actual encrypted content]
 *
 * The salt is regenerated every time the password is changed.
 * The IV is different every time.
 */

using namespace tencrypt;

Encryptor *encryptor_ptr;

Encryptor::Encryptor()
{
    encryptor_ptr = this;
    m_salt = CryptoPP::SecByteBlock(SALT_SIZE);
    m_key = CryptoPP::SecByteBlock(KEY_SIZE);
    m_key_set = false;
    m_decrypt_iv = CryptoPP::SecByteBlock(IV_SIZE);
    m_encrypted_str = std::string();
}

Encryptor::~Encryptor() {}

Encryptor *Encryptor::instance()
{
    return encryptor_ptr;
}

void Encryptor::reset()
{
    m_salt.Assign(CryptoPP::SecByteBlock(SALT_SIZE));
    m_key.Assign(CryptoPP::SecByteBlock(KEY_SIZE));
    m_key_set = false;
    m_decrypt_iv.Assign(CryptoPP::SecByteBlock(IV_SIZE));
    m_encrypted_str.clear();
    qDebug() << "Encryptor reset.";
}

void Encryptor::regenerate_salt()
{
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock(m_salt.data(), SALT_SIZE);
    qDebug() << "Salt regenerated.";
}

void Encryptor::set_key(std::string const &plaintext)
{
    CryptoPP::SecByteBlock byte_block(reinterpret_cast<CryptoPP::byte const *>(plaintext.data()), plaintext.size());
    CryptoPP::Scrypt scrypt;
    m_key_set = true;

    // This is supposed to be computationally expensive to make it hard to brute force attack. It SHOULD take a
    // reasonable amount of time. Unfortunately it takes WAY longer on Windows compared with Linux.
    scrypt.DeriveKey(
        m_key.data(), KEY_SIZE, byte_block.data(), byte_block.size(), m_salt.data(), SALT_SIZE, 1 << 15, 8, 16);
}

void Encryptor::set_salt(std::string const &salt_str)
{
    m_salt.Assign(CryptoPP::SecByteBlock(reinterpret_cast<CryptoPP::byte const *>(salt_str.data()), SALT_SIZE));
    qDebug() << "New salt set.";
}

void Encryptor::parse_encrypted_string(std::string &encrypted)
{
    set_salt(encrypted.substr(0, SALT_SIZE));
    encrypted.erase(0, SALT_SIZE);
    set_decrypt_iv(encrypted.substr(0, IV_SIZE));
    encrypted.erase(0, IV_SIZE);
}

// Requires a salt be already set.
void Encryptor::encrypt(std::string const &plaintext, std::string &encrypted)
{
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock iv(IV_SIZE);
    CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;

    prng.GenerateBlock(iv, IV_SIZE);
    encryptor.SetKeyWithIV(m_key.data(), KEY_SIZE, iv, IV_SIZE);

    // Put content to be encrypted in the encryption AND authentication channel.
    CryptoPP::AuthenticatedEncryptionFilter encryption_filter(
        encryptor, new CryptoPP::StringSink(encrypted), false, TAG_SIZE);
    encryption_filter.ChannelPut("", reinterpret_cast<CryptoPP::byte const *>(plaintext.data()), plaintext.size());
    encryption_filter.ChannelMessageEnd("");

    // Prepend the IV.
    std::string const iv_str(reinterpret_cast<char const *>(iv.data()), IV_SIZE);
    encrypted.insert(0, iv_str);

    // Prepend the salt.
    std::string const salt_str(reinterpret_cast<char const *>(m_salt.data()), SALT_SIZE);
    encrypted.insert(0, salt_str);
}

void Encryptor::set_decrypt_iv(std::string const &iv_str)
{
    m_decrypt_iv.Assign(CryptoPP::SecByteBlock(reinterpret_cast<CryptoPP::byte const *>(iv_str.data()), IV_SIZE));
    qDebug() << "New decrypt IV set.";
}

// Requires a salt and IV already set.
std::optional<std::string> Encryptor::decrypt(std::string const &encrypted)
{
    try {
        CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(m_key.data(), KEY_SIZE, m_decrypt_iv, IV_SIZE);

        std::string encrypted_data = encrypted.substr(0, encrypted.size() - TAG_SIZE);
        std::string mac_value = encrypted.substr(encrypted.size() - TAG_SIZE);

        CryptoPP::AuthenticatedDecryptionFilter decryption_filter(decryptor, NULL,
            CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
                CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
            TAG_SIZE);

        // There is only data in the encrypted and authenticated channel.
        decryption_filter.ChannelPut("", reinterpret_cast<CryptoPP::byte const *>(mac_value.data()), mac_value.size());
        decryption_filter.ChannelPut(
            "", reinterpret_cast<CryptoPP::byte const *>(encrypted_data.data()), encrypted_data.size());
        decryption_filter.ChannelMessageEnd("");

        // Test the authenticity of the data.
        bool const success = decryption_filter.GetLastResult();
        if (!success)
            return std::nullopt;

        // Allocate enough space for the decrypted content.
        std::size_t n = (std::size_t)-1;
        decryption_filter.SetRetrievalChannel("");
        n = static_cast<std::size_t>(decryption_filter.MaxRetrievable());
        std::string plaintext;
        plaintext.resize(n);

        if (n > 0)
            decryption_filter.Get(reinterpret_cast<CryptoPP::byte *>(plaintext.data()), n);

        return std::optional(plaintext);
    }
    catch (CryptoPP::HashVerificationFilter::HashVerificationFailed const &e) {
        return std::nullopt;
    }
}
