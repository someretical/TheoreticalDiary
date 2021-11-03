/*
 * This file is part of Theoretical Diary.
 *
 * Theoretical Diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Theoretical Diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "encryptor.h"

#include <QDebug>
#include <aes.h>
#include <files.h>
#include <filters.h>
#include <gcm.h>
#include <osrng.h>
#include <scrypt.h>

// The encryption algorithm was adapted from the AES-GCM-Test.zip file from
// https://www.cryptopp.com/wiki/Advanced_Encryption_Standard#Downloads
// The hashing algorithm was adopted from
// https://www.cryptopp.com/wiki/Scrypt#OpenMP

Encryptor::Encryptor() {
  salt = new CryptoPP::SecByteBlock(SALT_SIZE);
  key = new CryptoPP::SecByteBlock(KEY_SIZE);
  key_set = false;
  decrypt_iv = new CryptoPP::SecByteBlock(IV_SIZE);
}

Encryptor::~Encryptor() {
  delete salt;
  delete key;
  delete decrypt_iv;
}

void Encryptor::reset() {
  salt->Assign(CryptoPP::SecByteBlock(SALT_SIZE));
  key->Assign(CryptoPP::SecByteBlock(KEY_SIZE));
  key_set = false;
  decrypt_iv->Assign(CryptoPP::SecByteBlock(IV_SIZE));
}

void Encryptor::regenerate_salt() {
  CryptoPP::AutoSeededRandomPool prng;
  prng.GenerateBlock(salt->data(), SALT_SIZE);
}

void Encryptor::set_key(const std::string &plaintext) {
  CryptoPP::SecByteBlock byte_block(
      reinterpret_cast<const CryptoPP::byte *>(plaintext.data()),
      plaintext.size());
  CryptoPP::Scrypt scrypt;
  key_set = true;

  // This is supposed to be computationally expensive to make it hard to brute
  // force attack. It SHOULD take a reasonable amount of time.
  scrypt.DeriveKey(key->data(), KEY_SIZE, byte_block.data(), byte_block.size(),
                   salt->data(), SALT_SIZE, 1 << 17, 8, 16);
}

void Encryptor::set_salt(const std::string &salt_str) {
  salt->Assign(CryptoPP::SecByteBlock(
      reinterpret_cast<const CryptoPP::byte *>(salt_str.data()), SALT_SIZE));
}

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

// Requires a salt be already set
void Encryptor::encrypt(const std::string &plaintext, std::string &encrypted) {
  CryptoPP::AutoSeededRandomPool prng;
  CryptoPP::SecByteBlock iv(IV_SIZE);
  CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;

  prng.GenerateBlock(iv, IV_SIZE);
  encryptor.SetKeyWithIV(key->data(), KEY_SIZE, iv, IV_SIZE);

  // Put content to be encrypted in the encryption AND authentication channel.
  CryptoPP::AuthenticatedEncryptionFilter encryption_filter(
      encryptor, new CryptoPP::StringSink(encrypted), false, TAG_SIZE);
  encryption_filter.ChannelPut(
      "", reinterpret_cast<const CryptoPP::byte *>(plaintext.data()),
      plaintext.size());
  encryption_filter.ChannelMessageEnd("");

  // Prepend the IV
  std::string iv_str(reinterpret_cast<const char *>(iv.data()), IV_SIZE);
  encrypted.insert(0, iv_str);

  // Prepend the salt
  std::string salt_str(reinterpret_cast<const char *>(salt->data()), SALT_SIZE);
  encrypted.insert(0, salt_str);
}

void Encryptor::set_decrypt_iv(const std::string &iv_str) {
  decrypt_iv->Assign(CryptoPP::SecByteBlock(
      reinterpret_cast<const CryptoPP::byte *>(iv_str.data()), IV_SIZE));
}

// Requires a salt and IV already set
std::optional<std::string> Encryptor::decrypt(const std::string &encrypted) {
  try {
    CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(key->data(), KEY_SIZE, *decrypt_iv, IV_SIZE);

    std::string encrypted_data =
        encrypted.substr(0, encrypted.size() - TAG_SIZE);
    std::string mac_value = encrypted.substr(encrypted.size() - TAG_SIZE);

    CryptoPP::AuthenticatedDecryptionFilter decryption_filter(
        decryptor, NULL,
        CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
        TAG_SIZE);

    // There is only data in the encrypted and authenticated channel.
    decryption_filter.ChannelPut(
        "", reinterpret_cast<const CryptoPP::byte *>(mac_value.data()),
        mac_value.size());
    decryption_filter.ChannelPut(
        "", reinterpret_cast<const CryptoPP::byte *>(encrypted_data.data()),
        encrypted_data.size());
    decryption_filter.ChannelMessageEnd("");

    // Test the authenticity of the data.
    bool success = decryption_filter.GetLastResult();
    if (!success)
      return std::nullopt;

    // Allocate enough space for the decrypted content.
    std::size_t n = (std::size_t)-1;
    decryption_filter.SetRetrievalChannel("");
    n = static_cast<std::size_t>(decryption_filter.MaxRetrievable());
    std::string plaintext;
    plaintext.resize(n);

    if (n > 0)
      decryption_filter.Get(
          reinterpret_cast<CryptoPP::byte *>(plaintext.data()), n);

    return std::make_optional<std::string>(plaintext);
  } catch (const CryptoPP::HashVerificationFilter::HashVerificationFailed &e) {
    return std::nullopt;
  }
}
