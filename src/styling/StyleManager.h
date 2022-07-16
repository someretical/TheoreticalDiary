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

#include "../util/Constants.h"
#include <QtWidgets>

#ifndef THEORETICAL_DIARY_STYLEMANAGER_H
#define THEORETICAL_DIARY_STYLEMANAGER_H

class StyleManager {
public:
    StyleManager();
    ~StyleManager();
    static StyleManager *instance();

    void update_style();
    TD::Theme theme();

    QHash<int, QColor> m_colourmap;

private:
    inline void set_colour(TD::ColourRole role, const QColor &colour)
    {
        m_colourmap[static_cast<int>(role)] = colour;
    }
};

#endif // THEORETICAL_DIARY_STYLEMANAGER_H
