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

#include <cryptlib.h>
#include <optional>
#include <secblock.h>

// Sizes are in BYTES not bits
int const TAG_SIZE = 16;
int const SALT_SIZE = 64;
int const KEY_SIZE = 32;
int const IV_SIZE = 12;

class Encryptor {
public:
    Encryptor();
    ~Encryptor();

    void reset();
    void regenerate_salt();
    void set_key(std::string const &plaintext);
    void set_salt(std::string const &salt_str);
    void set_decrypt_iv(std::string const &iv_str);
    void encrypt(std::string const &plaintext, std::string &encrypted);
    std::optional<std::string> decrypt(std::string const &encrypted);

    bool key_set;
    std::string *encrypted_str;

private:
    CryptoPP::SecByteBlock *salt;
    CryptoPP::SecByteBlock *key;
    CryptoPP::SecByteBlock *decrypt_iv;
};

#endif // ENCRYPTOR_H
