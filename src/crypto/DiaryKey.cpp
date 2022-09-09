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

#include "DiaryKey.h"
#include "crypto/Random.h"
#include "crypto/botan_all.h"

#include <QDataStream>

DiaryKey::DiaryKey(const qint32 rounds, const quint64 memory)
    : m_key(KEY_SIZE), m_salt(SALT_SIZE, '\0'), m_rounds(rounds), m_memory(memory)
{
}

DiaryKey::~DiaryKey() = default;

auto DiaryKey::getKey() -> Botan::secure_vector<char> &
{
    return m_key;
}

/**
 * Sets the password. This function does not generate the hashed key!
 *
 * @param password
 */
void DiaryKey::setPassword(const QString &password)
{
    auto ba = password.toUtf8();
    m_password.assign(ba.begin(), ba.end());
    ba.fill('\0');
}

/**
 * Derives the key from the password. Requires the password to be set prior.
 */
void DiaryKey::generateKey()
{
    randomGen().randomize(m_salt);
    Botan::argon2(reinterpret_cast<uint8_t *>(m_key.data()), m_key.size(), m_password.data(), m_password.size(),
        reinterpret_cast<const uint8_t *>(m_salt.data()), m_salt.size(), nullptr, 0 /* No secret */, nullptr,
        0 /* No AD */, 0 /* argon2d */, 1 /* 1 thread only */, m_memory, m_rounds);
}

/**
 * Exports the key derivation function settings.
 * @return
 */
auto DiaryKey::serialise() -> QByteArray
{
    QByteArray ba;
    QDataStream s(ba);
    s << m_rounds << m_memory << m_salt;
    return ba;
}

/**
 * Gets the number of rounds required for a certain amount of time
 *
 * @param ms Required derivation time
 * @return Recommended number of rounds
 */
int DiaryKey::benchmark(int ms)
{
    // TODO implement
    return 0;
}