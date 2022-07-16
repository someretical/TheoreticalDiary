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

#include "StyleManager.h"
#include "../util/Util.h"
#include "DarkStyle.h"
#include "LightStyle.h"
#include <Logger.h>

StyleManager *single_instance = nullptr;

StyleManager::StyleManager()
{
    single_instance = this;
    update_style();
}

StyleManager::~StyleManager() {}

StyleManager *StyleManager::instance()
{
    return single_instance;
}

void StyleManager::update_style()
{
    QPixmapCache::clear();

    // There is no memory leak here as Qt deletes the old style
    // I made sure to actually check the source code of QApplication
    if (theme() == TD::Theme::Dark) {
        set_colour(TD::ColourRole::Unknown, QStringLiteral("#2F2F32"));
        set_colour(TD::ColourRole::VeryBad, QStringLiteral("#C43F31"));
        set_colour(TD::ColourRole::Bad, QStringLiteral("#DB9837"));
        set_colour(TD::ColourRole::Ok, QStringLiteral("#F0C400"));
        set_colour(TD::ColourRole::Good, QStringLiteral("#608A22"));
        set_colour(TD::ColourRole::VeryGood, QStringLiteral("#1F8023"));
        set_colour(TD::ColourRole::Text, QStringLiteral("#CACBCE"));

        auto s = new DarkStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
        LOG_INFO() << "Switched to dark theme";
    }
    else {
        set_colour(TD::ColourRole::Unknown, QStringLiteral("#C9C9CF"));
        set_colour(TD::ColourRole::VeryBad, QStringLiteral("#C43F31"));
        set_colour(TD::ColourRole::Bad, QStringLiteral("#E07F16"));
        set_colour(TD::ColourRole::Ok, QStringLiteral("#FFD30F"));
        set_colour(TD::ColourRole::Good, QStringLiteral("#5EA10E"));
        set_colour(TD::ColourRole::VeryGood, QStringLiteral("#118F17"));
        set_colour(TD::ColourRole::Text, QStringLiteral("#1D1D20"));

        auto s = new LightStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
        LOG_INFO() << "Switched to light theme";
    }
}

TD::Theme StyleManager::theme()
{
    auto settings = util::settings();
    settings.beginGroup(QStringLiteral("General"));

    if (settings.contains(QStringLiteral("Theme"))) {
        return static_cast<TD::Theme>(settings.value(QStringLiteral("Theme")).toInt());
    }
    else {
        settings.setValue(QStringLiteral("Theme"), static_cast<int>(TD::Theme::Dark));
        return TD::Theme::Dark;
    }
}
