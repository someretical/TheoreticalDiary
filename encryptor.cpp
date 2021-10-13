/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "encryptor.h"

#include <aes.h>
#include <cryptlib.h>
#include <cstddef>
#include <files.h>
#include <filters.h>
#include <gcm.h>
#include <osrng.h>
#include <sha.h>
#include <string>
#include <vector>

// This code was adapted from the AES-GCM-Test.zip file from
// https://www.cryptopp.com/wiki/Advanced_Encryption_Standard#Downloads

#define TAG_SIZE 16

/**
 * Calculate a 256 bit hash using SHA256 to represent the password
 * 256 bits is needed because AES-GCM requires a 32 byte key
 */
void Encryptor::get_hash(const std::string &password,
                         std::vector<CryptoPP::byte> &output) {
  CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];

  CryptoPP::SHA256 hash;
  hash.CalculateDigest(digest, (CryptoPP::byte *)password.data(),
                       password.size());

  for (auto byte : digest)
    output.push_back(byte);
}

/**
 * Encrypts a string.
 */
void Encryptor::encrypt(const std::vector<CryptoPP::byte> &key,
                        const std::string &decrypted, std::string &encrypted) {
  // Cryptographically secure source of entropy.
  CryptoPP::AutoSeededRandomPool prng;
  CryptoPP::SecByteBlock iv(12);
  prng.GenerateBlock(iv, iv.size());

  CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
  encryptor.SetKeyWithIV(key.data(), key.size(), iv, iv.size());

  // Put content to be encrypted in the encryption AND authentication channel.
  CryptoPP::AuthenticatedEncryptionFilter encryption_filter(
      encryptor, new CryptoPP::StringSink(encrypted), false, TAG_SIZE);
  encryption_filter.ChannelPut("", (const CryptoPP::byte *)decrypted.data(),
                               decrypted.size());
  encryption_filter.ChannelMessageEnd("");

  /**
   * The IV needs to be prepended to the encrypted content.
   * It does not need to be encrypted.
   */
  std::string token(reinterpret_cast<const char *>(&iv[0]), iv.size());
  encrypted.insert(0, token);
}

bool Encryptor::decrypt(const std::vector<CryptoPP::byte> &key,
                        std::string &encrypted, std::string &decrypted) {
  try {
    // Retrive IV from encrypted message
    std::string token = encrypted.substr(0, 12);
    encrypted.erase(0, 12);

    CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
    CryptoPP::SecByteBlock iv(
        reinterpret_cast<const CryptoPP::byte *>(&token[0]), token.size());
    decryptor.SetKeyWithIV(key.data(), key.size(), iv, iv.size());

    std::string encrypted_data =
        encrypted.substr(0, encrypted.size() - TAG_SIZE);
    std::string mac_value = encrypted.substr(encrypted.size() - TAG_SIZE);

    CryptoPP::AuthenticatedDecryptionFilter decryption_filter(
        decryptor, NULL,
        CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION,
        TAG_SIZE);

    // There is only data in the encrypted and authenticated channel.
    decryption_filter.ChannelPut("", (const CryptoPP::byte *)mac_value.data(),
                                 mac_value.size());
    decryption_filter.ChannelPut("",
                                 (const CryptoPP::byte *)encrypted_data.data(),
                                 encrypted_data.size());
    decryption_filter.ChannelMessageEnd("");

    // Test the authenticity of the data.
    bool success = decryption_filter.GetLastResult();
    if (!success)
      return false;

    // Allocate enough space for the decrypted content.
    std::size_t n = (std::size_t)-1;
    decryption_filter.SetRetrievalChannel("");
    n = (std::size_t)decryption_filter.MaxRetrievable();
    decrypted.resize(n);

    if (n > 0)
      decryption_filter.Get((CryptoPP::byte *)decrypted.data(), n);

    return true;
  } catch (CryptoPP::HashVerificationFilter::HashVerificationFailed &e) {
    return false;
  }
}
