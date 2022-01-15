/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "internalmanager.h"
#include "../gui/styles/dark/darkstyle.h"
#include "../gui/styles/light/lightstyle.h"

InternalManager *i_m_ptr;

InternalManager::InternalManager()
{
    i_m_ptr = this;
    app_busy = false;
    internal_diary_changed = false;
    diary_file_changed = false;

    settings = new QSettings(QString("%1/%2").arg(data_location(), "config.ini"), QSettings::IniFormat, this);
    init_settings(false);

    inactive_filter = new InactiveFilter(settings->value("lock_timeout").toLongLong(), this);
    QApplication::instance()->installEventFilter(inactive_filter);

    start_update_theme();
}

InternalManager::~InternalManager()
{
    delete settings;
    delete inactive_filter;
}

InternalManager *InternalManager::instance()
{
    return i_m_ptr;
}

void InternalManager::init_settings(const bool force_reset)
{
    if (!settings->contains("sync_enabled") || force_reset)
        settings->setValue("sync_enabled", false);

    if (!settings->contains("theme") || force_reset)
        settings->setValue("theme", static_cast<int>(td::Theme::Dark));

    if (!settings->contains("lock_timeout") || force_reset)
        settings->setValue("lock_timeout", static_cast<qint64>(300000 /* 5 minutes */));
}

td::Theme InternalManager::get_theme()
{
    return static_cast<td::Theme>(settings->value("theme").toInt());
}

QString InternalManager::get_theme_str()
{
    return QString(td::Theme::Light == get_theme() ? "light" : "dark");
}

QString InternalManager::data_location()
{
    return QString("%1/%2").arg(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QApplication::applicationName());
}

void InternalManager::start_busy_mode(int const line, std::string const &func, std::string const &file)
{
    qDebug() << "!!! Attempted start busy:" << line << func.data() << file.data();

    if (app_busy)
        return;

    QApplication::instance()->installEventFilter(&busy_filter);
    app_busy = true;
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void InternalManager::end_busy_mode(int const line, std::string const &func, std::string const &file)
{
    qDebug() << "!!! Attempted end busy:" << line << func.data() << file.data();

    if (!app_busy)
        return;

    QApplication::instance()->removeEventFilter(&busy_filter);
    app_busy = false;
    QGuiApplication::restoreOverrideCursor();
}

void InternalManager::start_update_theme()
{
    QPixmapCache::clear();
    state_color_palette = StateColorPalette();

    if (get_theme() == td::Theme::Dark) {
        state_color_palette.initDefaultPaletteDark();
        auto s = new DarkStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
    }
    else {
        state_color_palette.initDefaultPaletteLight();
        auto s = new LightStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
    }

    emit update_theme();
}
