/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2021  someretical
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

InternalManager *i_m_ptr;

InternalManager::InternalManager(const td::Theme t)
{
    i_m_ptr = this;
    theme = t;
    app_busy = false;
    internal_diary_changed = false;
    diary_file_changed = false;

    settings = new QSettings(QString("%1/%2").arg(data_location(), "config.ini"), QSettings::IniFormat, this);
    if (!settings->contains("sync_enabled"))
        settings->setValue("sync_enabled", false);
}

InternalManager::~InternalManager()
{
    delete settings;
}

InternalManager *InternalManager::instance()
{
    return i_m_ptr;
}

QString InternalManager::get_theme()
{
    return QString(td::Theme::Light == theme ? "light" : "dark");
}

QString InternalManager::data_location()
{
    return QString("%1/%2").arg(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QApplication::applicationName());
}

void InternalManager::start_busy_mode(int const line, std::string const &func, std::string const &file)
{
    qDebug() << "!!! Attempted start busy:" << line << QString::fromStdString(func) << QString::fromStdString(file);

    if (app_busy)
        return;

    QApplication::instance()->installEventFilter(&busy_filter);
    app_busy = true;
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void InternalManager::end_busy_mode(int const line, std::string const &func, std::string const &file)
{
    qDebug() << "!!! Attempted end busy:" << line << QString::fromStdString(func) << QString::fromStdString(file);

    if (!app_busy)
        return;

    QApplication::instance()->removeEventFilter(&busy_filter);
    app_busy = false;
    QGuiApplication::restoreOverrideCursor();
}
