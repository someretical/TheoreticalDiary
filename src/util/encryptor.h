/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2021  someretical
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

#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QObject>
#include <cryptlib.h>
#include <cstddef>
#include <optional>
#include <secblock.h>
#include <string>

// Sizes are in BYTES not bits
const int TAG_SIZE = 16;
const int SALT_SIZE = 64;
const int KEY_SIZE = 32;
const int IV_SIZE = 12;

class Encryptor {
public:
  Encryptor();
  ~Encryptor();

  void reset();
  void regenerate_salt();
  void set_key(const std::string &plaintext);
  void set_salt(const std::string &salt_str);
  void set_decrypt_iv(const std::string &iv_str);
  void encrypt(const std::string &plaintext, std::string &encrypted);
  std::optional<std::string> decrypt(const std::string &encrypted);

  bool key_set;

private:
  CryptoPP::SecByteBlock *salt;
  CryptoPP::SecByteBlock *key;
  CryptoPP::SecByteBlock *decrypt_iv;
};

#endif // ENCRYPTOR_H
