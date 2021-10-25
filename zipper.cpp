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

#include "zipper.h"

#include <files.h>
#include <gzip.h>
#include <string>

void Zipper::zip(std::string &compressed, std::string &decompressed) {
  CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
  zipper.Put(reinterpret_cast<CryptoPP::byte *>(decompressed.data()),
             decompressed.size());
  zipper.MessageEnd();
}

bool Zipper::unzip(std::string &compressed, std::string &decompressed) {
  try {
    CryptoPP::Gunzip unzipper(new CryptoPP::StringSink(decompressed));
    unzipper.Put(reinterpret_cast<CryptoPP::byte *>(compressed.data()),
                 compressed.size());
    unzipper.MessageEnd();
    return true;
  } catch (const CryptoPP::Exception &e) {
    return false;
  }
}
