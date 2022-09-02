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

#include <QDataStream>

#include "DiaryKey.h"
#include "crypto/Random.h"
#include "crypto/botan_all.h"

DiaryKey::DiaryKey(
    const QString &password, const qint32 rounds = DEFAULT_ROUNDS, const quint64 memory = DEFAULT_MEMORY_SIZE)
    : m_key(KEY_SIZE), m_salt(SALT_SIZE, '\0'), m_rounds(rounds), m_memory(memory)
{
    generateKey(password);
}

DiaryKey::~DiaryKey() = default;

auto DiaryKey::getKey() -> Botan::secure_vector<char> &
{
    return m_key;
}

void DiaryKey::generateKey(const QString &password)
{
    randomGen().randomize(m_salt);
    auto ba = password.toUtf8();
    Botan::argon2(reinterpret_cast<uint8_t *>(m_key.data()), m_key.size(), ba.data(), ba.size(),
        reinterpret_cast<const uint8_t *>(m_salt.data()), m_salt.size(), nullptr, 0 /* No secret */, nullptr,
        0 /* No AD */, 0 /* argon2d */, 1 /* 1 thread only */, m_memory, m_rounds);
    ba.fill('\0');
}

QByteArray DiaryKey::serialise()
{
    QByteArray ba;
    QDataStream s(ba);
    s << m_rounds << m_memory << m_salt;
    return ba;
}

int DiaryKey::benchmark(int ms)
{
    return 0;
}