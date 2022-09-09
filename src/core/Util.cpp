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

#include "core/Util.h"

#include <QDir>

/**
 * Get the application data path used to store configuration files
 *
 * @return
 */
auto dataPath() -> QString
{
#ifdef QT_DEBUG
    return QStringLiteral("%1/.theoreticaldiary/debug").arg(QDir::homePath());
#else
    return QString("%1/.theoreticaldiary").arg(QDir::homePath());
#endif
}

/**
 * Get the ordinal suffix for a number. E.g. -st, -nd, -rd, -th
 *
 * @param number
 * @return
 */
auto getOrdinalSuffix(int const number) -> QString
{
    switch (number) {
    case 1:
    case 21:
    case 31:
        return "st";
    case 2:
    case 22:
        return "nd";
    case 3:
    case 23:
        return "rd";
    default:
        return "th";
    }
}