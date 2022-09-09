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

#include "gui/styling/DarkStyle.h"
#include "gui/styling/LightStyle.h"
#include "gui/styling/StyleManager.h"
#include "core/Config.h"

#include <Logger.h>
#include <QApplication>
#include <QPixmapCache>


StyleManager::StyleManager()
{
    setDarkColour(TD::ColourRole::Unknown, 0x2F2F32);
    setDarkColour(TD::ColourRole::VeryBad, 0xC43F31);
    setDarkColour(TD::ColourRole::Bad, 0xDB9837);
    setDarkColour(TD::ColourRole::Ok, 0xF0C400);
    setDarkColour(TD::ColourRole::Good, 0x608A22);
    setDarkColour(TD::ColourRole::VeryGood, 0x1F8023);
    setDarkColour(TD::ColourRole::Text, 0xCACBCE);

    setLightColour(TD::ColourRole::Unknown, 0xC9C9CF);
    setLightColour(TD::ColourRole::VeryBad, 0xC43F31);
    setLightColour(TD::ColourRole::Bad, 0xE07F16);
    setLightColour(TD::ColourRole::Ok, 0xFFD30F);
    setLightColour(TD::ColourRole::Good, 0x5EA10E);
    setLightColour(TD::ColourRole::VeryGood, 0x118F17);
    setLightColour(TD::ColourRole::Text, 0x1D1D20);

    updateStyle();
}

StyleManager::~StyleManager() = default;

/**
 * Updates the display style. Call config()->set(Config::GUI_Theme, TD::Theme) first
 */
void StyleManager::updateStyle()
{
    QPixmapCache::clear();

    // There is no memory leak here as Qt deletes the old style
    // I made sure to actually check the source code of QApplication
    if (config()->get(Config::GUI_Theme).toInt() == TD::Theme::Dark) {
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

/**
 * Get the associated colour for a rating
 * @param rating
 * @return
 */
auto StyleManager::getRatingColour(const DiaryRating::Rating rating) -> QColor
{
    switch (rating) {
    case DiaryRating::Rating::Unknown:
        return m_colourMap[TD::ColourRole::Unknown];
    case DiaryRating::Rating::VeryBad:
        return m_colourMap[TD::ColourRole::VeryBad];
    case DiaryRating::Rating::Bad:
        return m_colourMap[TD::ColourRole::Bad];
    case DiaryRating::Rating::Ok:
        return m_colourMap[TD::ColourRole::Ok];
    case DiaryRating::Rating::Good:
        return m_colourMap[TD::ColourRole::Good];
    case DiaryRating::Rating::VeryGood:
        return m_colourMap[TD::ColourRole::VeryGood];
    }
}

/**
 * Get the colour of the text that should be drawn on top of the rating
 * @param rating
 * @return
 */
auto StyleManager::getTextColourFromTheme(const DiaryRating::Rating rating) -> QColor
{
    switch (rating) {
    case DiaryRating::Rating::Unknown:
        if (config()->get(Config::GUI_Theme).toInt() == TD::Theme::Light)
            return m_lightThemeMap[TD::ColourRole::Text];
        else
            return m_darkThemeMap[TD::ColourRole::Text];
    case DiaryRating::Rating::VeryBad:
    case DiaryRating::Rating::Bad:
    case DiaryRating::Rating::Ok:
    case DiaryRating::Rating::Good:
        return m_lightThemeMap[TD::ColourRole::Text];
    case DiaryRating::Rating::VeryGood:
        return m_darkThemeMap[TD::ColourRole::Text];
    }
}
