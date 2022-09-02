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

#ifndef KEEPASSX_RANDOM_H
#define KEEPASSX_RANDOM_H

#include <QSharedPointer>

#include "botan_all.h"

class Random {
public:
    static auto instance() -> Random &
    {
        static Random r;
        return r;
    }

    void randomize(QByteArray &ba);
    auto randomArray(int len) -> QByteArray;
    auto randomUInt(quint32 limit) -> quint32;
    auto randomUIntRange(quint32 min, quint32 max) -> quint32;
    auto getRng() -> QSharedPointer<Botan::RandomNumberGenerator>;

private:
    Random();
    ~Random();

    QSharedPointer<Botan::RandomNumberGenerator> m_rng;
};

static inline Random &randomGen()
{
    return Random::instance();
}

#endif // KEEPASSX_RANDOM_H
