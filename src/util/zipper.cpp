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

#include "zipper.h"

void Zipper::zip(std::string &compressed, std::string &decompressed)
{
    CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
    zipper.Put(reinterpret_cast<CryptoPP::byte *>(decompressed.data()), decompressed.size());
    zipper.MessageEnd();
}

bool Zipper::unzip(std::string &compressed, std::string &decompressed)
{
    try {
        CryptoPP::Gunzip unzipper(new CryptoPP::StringSink(decompressed));
        unzipper.Put(reinterpret_cast<CryptoPP::byte *>(compressed.data()), compressed.size());
        unzipper.MessageEnd();
        return true;
    }
    catch (CryptoPP::Exception const &e) {
        return false;
    }
}
