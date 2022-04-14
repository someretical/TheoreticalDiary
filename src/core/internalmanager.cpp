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
    m_app_busy = false;
    m_internal_diary_changed = false;
    m_diary_file_changed = false;

    m_settings = new QSettings(QString("%1/%2").arg(data_location(), "config.ini"), QSettings::IniFormat, this);
    init_settings(false);

    m_inactive_filter = new InactiveFilter(m_settings->value("lock_timeout").toLongLong(), this);
    QApplication::instance()->installEventFilter(m_inactive_filter);

    start_update_theme();
}

InternalManager::~InternalManager()
{
    delete m_settings;
    delete m_inactive_filter;
}

InternalManager *InternalManager::instance()
{
    return i_m_ptr;
}

void InternalManager::init_settings(const bool force_reset)
{
    if (!m_settings->contains("sync_enabled") || force_reset)
        m_settings->setValue("sync_enabled", false);

    if (!m_settings->contains("theme") || force_reset)
        m_settings->setValue("theme", static_cast<int>(td::Theme::Dark));

    if (!m_settings->contains("lock_timeout") || force_reset)
        m_settings->setValue("lock_timeout", static_cast<qint64>(300000 /* 5 minutes */));

    if (!m_settings->contains("pie_slice_sorting") || force_reset)
        m_settings->setValue("pie_slice_sorting", static_cast<int>(td::settings::PieSliceSort::Days));
}

td::Theme InternalManager::get_theme(bool const opposite)
{
    if (opposite) {
        return static_cast<td::Theme>(m_settings->value("theme").toInt()) == td::Theme::Dark ? td::Theme::Light
                                                                                           : td::Theme::Dark;
    }
    else {
        return static_cast<td::Theme>(m_settings->value("theme").toInt());
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
    // Testing convenience
#ifdef QT_DEBUG
    return QString("%1/%2-DEBUG")
        .arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QApplication::applicationName());
#else
    return QString("%1/%2").arg(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QApplication::applicationName());
#endif
}

void InternalManager::start_busy_mode()
{
    if (m_app_busy)
        return;

    QApplication::instance()->installEventFilter(&m_busy_filter);
    m_app_busy = true;
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void InternalManager::end_busy_mode()
{
    if (!m_app_busy)
        return;

    QApplication::instance()->removeEventFilter(&m_busy_filter);
    m_app_busy = false;
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
    set_colour(td::ColourRole::VeryBad, QStringLiteral("#C43F31"));
    set_colour(td::ColourRole::Bad, QStringLiteral("#E07F16"));
    set_colour(td::ColourRole::Ok, QStringLiteral("#FFD30F"));
    set_colour(td::ColourRole::Good, QStringLiteral("#5EA10E"));
    set_colour(td::ColourRole::VeryGood, QStringLiteral("#118f17"));
    set_colour(td::ColourRole::Text, QStringLiteral("#1D1D20"));
}

void InternalManager::set_dark_palette()
{
    set_colour(td::ColourRole::Unknown, QStringLiteral("#2F2F32"));
    set_colour(td::ColourRole::VeryBad, QStringLiteral("#C43F31"));
    set_colour(td::ColourRole::Bad, QStringLiteral("#DB9837"));
    set_colour(td::ColourRole::Ok, QStringLiteral("#F0C400"));
    set_colour(td::ColourRole::Good, QStringLiteral("#608A22"));
    set_colour(td::ColourRole::VeryGood, QStringLiteral("#1F8023"));
    set_colour(td::ColourRole::Text, QStringLiteral("#CACBCE"));
}
