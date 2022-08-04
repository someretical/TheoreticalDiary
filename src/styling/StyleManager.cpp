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

#include <Logger.h>
#include <QApplication>
#include <QPixmapCache>

#include "DarkStyle.h"
#include "LightStyle.h"
#include "StyleManager.h"
#include "util/Util.h"

StyleManager *StyleManager::m_instance = nullptr;

StyleManager::StyleManager()
{
    m_instance = this;
    updateStyle();
}

StyleManager::~StyleManager() = default;

auto StyleManager::instance() -> StyleManager *
{
    return m_instance;
}

void StyleManager::updateStyle()
{
    QPixmapCache::clear();

    // There is no memory leak here as Qt deletes the old style
    // I made sure to actually check the source code of QApplication
    if (theme() == TD::Theme::Dark) {
        setColour(TD::ColourRole::Unknown, 0x2F2F32);
        setColour(TD::ColourRole::VeryBad, 0xC43F31);
        setColour(TD::ColourRole::Bad, 0xDB9837);
        setColour(TD::ColourRole::Ok, 0xF0C400);
        setColour(TD::ColourRole::Good, 0x608A22);
        setColour(TD::ColourRole::VeryGood, 0x1F8023);
        setColour(TD::ColourRole::Text, 0xCACBCE);

        auto s = new DarkStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
        LOG_INFO() << "Switched to dark theme";
    }
    else {
        setColour(TD::ColourRole::Unknown, 0xC9C9CF);
        setColour(TD::ColourRole::VeryBad, 0xC43F31);
        setColour(TD::ColourRole::Bad, 0xE07F16);
        setColour(TD::ColourRole::Ok, 0xFFD30F);
        setColour(TD::ColourRole::Good, 0x5EA10E);
        setColour(TD::ColourRole::VeryGood, 0x118F17);
        setColour(TD::ColourRole::Text, 0x1D1D20);

        auto s = new LightStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
        LOG_INFO() << "Switched to light theme";
    }
}

auto StyleManager::theme() -> TD::Theme
{
    auto settings = util::settings();
    settings.beginGroup("General");

    if (settings.contains("Theme")) {
        return static_cast<TD::Theme>(settings.value("Theme").toInt());
    }
    else {
        settings.setValue("Theme", static_cast<int>(TD::Theme::Dark));
        return TD::Theme::Dark;
    }
}
