/*
 *  Copyright (C) 2021 KeePassXC Team <team@keepassxc.org>
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

#ifndef KEEPASSXC_STATECOLORPALETTE_H
#define KEEPASSXC_STATECOLORPALETTE_H

#include <QColor>
#include <QHash>
#include <QObject>

// TODO fix the really janky workaround keeping this class separate.
// Probably merge it into the internal manager class some time.

/**
 * Extended color palette for indicating custom widget states.
 */
class StateColorPalette {
    Q_GADGET

public:
    StateColorPalette();

    enum ColorRole { Error, Warning, Info, Incomplete, True, False, Unknown, VeryBad, Bad, Ok, Good, VeryGood };

    inline void setColor(ColorRole role, const QColor &color)
    {
        m_colorMap[role] = color;
    }

    inline QColor color(ColorRole role) const
    {
        return m_colorMap.value(role);
    }

    void initDefaultPaletteLight();
    void initDefaultPaletteDark();

private:
    QHash<ColorRole, QColor> m_colorMap;
};

#endif // KEEPASSXC_STATECOLORPALETTE_H
