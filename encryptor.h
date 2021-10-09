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

#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <cryptlib.h>
#include <cstddef>
#include <string>
#include <vector>

class Encryptor {
public:
  static void get_hash(std::string &password,
                       std::vector<CryptoPP::byte> &output);
  static void encrypt(std::vector<CryptoPP::byte> &key, std::string &decrypted,
                      std::string &encrypted);
  static bool decrypt(std::vector<CryptoPP::byte> &key, std::string &encrypted,
                      std::string &decrypted);
};

#endif // ENCRYPTOR_H
