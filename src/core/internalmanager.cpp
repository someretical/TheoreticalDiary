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

    if (!settings->contains("pie_slice_sorting") || force_reset)
        settings->setValue("pie_slice_sorting", static_cast<int>(td::settings::PieSliceSort::Days));
}

td::Theme InternalManager::get_theme(bool const opposite)
{
    if (opposite) {
        return static_cast<td::Theme>(settings->value("theme").toInt()) == td::Theme::Dark ? td::Theme::Light
                                                                                           : td::Theme::Dark;
    }
    else {
        return static_cast<td::Theme>(settings->value("theme").toInt());
    }
}

QString InternalManager::get_theme_str(bool const opposite)
{
    if (opposite) {
        return td::Theme::Light == get_theme() ? "dark" : "light";
    }
    else {
        return td::Theme::Light == get_theme() ? "light" : "dark";
    }
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

    // There is no memory leak here as Qt deletes the old style for us.
    // I made sure to actually check the source code of QApplication.
    if (get_theme() == td::Theme::Dark) {
        set_dark_palette();
        auto s = new DarkStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
    }
    else {
        set_light_palette();
        auto s = new LightStyle;
        QApplication::setPalette(s->standardPalette());
        QApplication::setStyle(s);
    }

    emit update_theme();
}

void InternalManager::set_light_palette()
{
    set_colour(td::ColourRole::Unknown, QStringLiteral("#C9C9CF"));
    set_colour(td::ColourRole::VeryBad, QStringLiteral("#7b1fa2"));
    set_colour(td::ColourRole::Bad, QStringLiteral("#5e35b1"));
    set_colour(td::ColourRole::Ok, QStringLiteral("#1976d2"));
    set_colour(td::ColourRole::Good, QStringLiteral("#0097a7"));
    set_colour(td::ColourRole::VeryGood, QStringLiteral("#4caf50"));
    set_colour(td::ColourRole::Text, QStringLiteral("#1D1D20"));
}

void InternalManager::set_dark_palette()
{
    set_colour(td::ColourRole::Unknown, QStringLiteral("#2F2F32"));
    set_colour(td::ColourRole::VeryBad, QStringLiteral("#7b1fa2"));
    set_colour(td::ColourRole::Bad, QStringLiteral("#5e35b1"));
    set_colour(td::ColourRole::Ok, QStringLiteral("#1976d2"));
    set_colour(td::ColourRole::Good, QStringLiteral("#0097a7"));
    set_colour(td::ColourRole::VeryGood, QStringLiteral("#4caf50"));
    set_colour(td::ColourRole::Text, QStringLiteral("#CACBCE"));
}
