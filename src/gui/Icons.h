/*
 *  Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
 *  Copyright (C) 2011 Felix Geyer <debfx@fobos.de>
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

#ifndef THEORETICAL_DIARY_ICONS_H
#define THEORETICAL_DIARY_ICONS_H

#include <QIcon>

class Icons {
public:
    Icons();
    auto icon(const QString &name, bool recolor = true, const QColor &overrideColor = QColor::Invalid) -> QIcon;
    auto onOffIcon(const QString &name, bool on, bool recolor = true) -> QIcon;

    static auto instance() -> Icons *;

private:
    static Icons *m_instance;
    QHash<QString, QIcon> m_iconCache;
};

inline auto icons() -> Icons *
{
    return Icons::instance();
}

#endif // THEORETICAL_DIARY_ICONS_H
