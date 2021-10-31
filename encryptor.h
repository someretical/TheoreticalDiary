/**
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

#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <cryptlib.h>
#include <cstddef>
#include <optional>
#include <secblock.h>
#include <sha.h>
#include <string>

class Encryptor {
public:
  Encryptor();
  ~Encryptor();

  void reset();
  void regenerate_salt();
  void set_key(const std::string &plaintext);
  void encrypt(const std::string &plaintext, std::string &encrypted);
  std::optional<std::string> decrypt(std::string &encrypted,
                                     const std::string &plaintext_key);

private:
  CryptoPP::SecByteBlock *salt;
  CryptoPP::SecByteBlock *key;
};

#endif // ENCRYPTOR_H
