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

#include <QColor>
#include <QHash>

#include "src/core/Constants.h"

#ifndef THEORETICAL_DIARY_STYLEMANAGER_H
#define THEORETICAL_DIARY_STYLEMANAGER_H

class StyleManager {
public:
    StyleManager();
    ~StyleManager();

    void updateStyle();
    static auto instance() -> StyleManager *;

    QHash<int, QColor> m_colourMap;

private:
    inline void setColour(TD::ColourRole role, const QColor &colour)
    {
        m_colourMap[static_cast<int>(role)] = colour;
    }

    static StyleManager *m_instance;
};

inline auto styleManager() -> StyleManager *
{
    return StyleManager::instance();
}

#endif // THEORETICAL_DIARY_STYLEMANAGER_H
