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

#ifndef THEORETICAL_DIARY_DIARYKEY_H
#define THEORETICAL_DIARY_DIARYKEY_H

#include <QString>

#include "crypto/botan_all.h"

const qint32 DEFAULT_ROUNDS = 10;
const quint64 DEFAULT_MEMORY_SIZE = 1 << 16; // In kibibytes
const qint32 SALT_SIZE = 32;                 // Bytes
const qint32 KEY_SIZE = 32;                  // Bytes

class DiaryKey {
public:
    explicit DiaryKey(const QString &password, qint32 rounds, quint64 memory);
    ~DiaryKey();

    auto getKey() -> Botan::secure_vector<char> &;
    void generateKey(const QString &password);
    auto serialise() -> QByteArray;

    auto benchmark(int ms) -> int;

private:
    Botan::secure_vector<char> m_key;
    QByteArray m_salt;
    qint32 m_rounds;
    quint64 m_memory;
};

#endif // THEORETICAL_DIARY_DIARYKEY_H
