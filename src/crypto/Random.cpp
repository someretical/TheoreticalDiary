/*
 *  Copyright (C) 2010 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Random.h"
#include "botan_all.h"

const quint32 QUINT32_MAX = 4294967295U;

Random::Random()
{
#ifdef BOTAN_HAS_SYSTEM_RNG
    m_rng.reset(new Botan::System_RNG);
#else
    m_rng.reset(new Botan::Autoseeded_RNG);
#endif
}

Random::~Random() = default;

auto Random::getRng() -> QSharedPointer<Botan::RandomNumberGenerator>
{
    return m_rng;
}

/**
 * Fills an existing QByteArray with random bytes
 *
 * @param ba
 */
void Random::randomize(QByteArray &ba)
{
    m_rng->randomize(reinterpret_cast<uint8_t *>(ba.data()), ba.size());
}

/**
 * Generates a QByteArray filled with random bytes
 *
 * @param len Length of QByteArray
 * @return
 */
auto Random::randomArray(int len) -> QByteArray
{
    QByteArray ba(len, '\0');
    randomize(ba);
    return ba;
}

/**
 * Generates a random unsigned integer between [0, limit)
 *
 * @param limit Upper limit (exclusive)
 * @return
 */
auto Random::randomUInt(quint32 limit) -> quint32
{
    if (limit == 0) {
        return 0;
    }

    quint32 rand;
    const quint32 ceil = QUINT32_MAX - (QUINT32_MAX % limit) - 1;

    // To avoid modulo bias make sure rand is below the largest number where rand%limit==0
    do {
        m_rng->randomize(reinterpret_cast<uint8_t *>(&rand), 4);
    } while (rand > ceil);

    return (rand % limit);
}

/**
 * Generates a random unsigned integer between [min, max)
 *
 * @param min Lower limit (inclusive)
 * @param max Upper limit (exclusive)
 * @return
 */
auto Random::randomUIntRange(quint32 min, quint32 max) -> quint32
{
    return min + randomUInt(max - min);
}
