/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THEORETICAL_DIARY_HASH_H
#define THEORETICAL_DIARY_HASH_H

#include <QByteArray>
#include <QScopedPointer>

#include "crypto/botan_all.h"

const int SHA256_BYTES = 32;
const int SHA512_BYTES = 64;

class Hash {
public:
    enum Algorithm { Sha256, Sha512 };

    explicit Hash(Algorithm algo, bool hmac = false);
    ~Hash();
    void addData(const QByteArray &data);
    auto result() const -> QByteArray;
    void setKey(const QByteArray &data);

    static auto hash(const QByteArray &data, Algorithm algo) -> QByteArray;
    static auto hmac(const QByteArray &data, const QByteArray &key, Algorithm algo) -> QByteArray;

private:
    QScopedPointer<Botan::HashFunction> m_hashFunction;
    QScopedPointer<Botan::MessageAuthenticationCode> m_hmacFunction;
};

#endif // THEORETICAL_DIARY_HASH_H
