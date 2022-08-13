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

#include <QCoreApplication>

#include "Config.h"
#include "core/Util.h"
#include "core/Constants.h"

#define QS QStringLiteral

struct ConfigDirective {
    QString name;
    QVariant defaultValue;
};

// clang-format off
static const QHash<Config::ConfigKey, ConfigDirective> configStrings = {
    {Config::RecentDiaries,   {QS("General/RecentDiaries"), {}}},
    {Config::MaximumRecentDiaries,   {QS("General/MaximumRecentDiaries"), 10}},

    {Config::GUI_Theme, {QS("GUI/Theme"), TD::Theme::Dark}},
    {Config::GUI_MainWindowGeometry, {QS("GUI/MainWindowGeometry"), {}}},
    {Config::GUI_MainWindowState,    {QS("GUI/MainWindowState"), {}}},
};
// clang-format on

Config *Config::m_instance = nullptr;

Config::Config(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings(QStringLiteral("%1/config.ini").arg(dataPath()), QSettings::IniFormat, this);
    m_instance = this;
}

auto Config::instance() -> Config *
{
    if (!m_instance)
        m_instance = new Config(QCoreApplication::instance());

    return m_instance;
}

auto Config::getDefault(const Config::ConfigKey key) -> QVariant
{
    return configStrings[key].defaultValue;
}

auto Config::get(const Config::ConfigKey key) -> QVariant
{
    return m_settings->value(configStrings[key].name, configStrings[key].defaultValue);
}

void Config::set(const Config::ConfigKey key, const QVariant &value)
{
    m_settings->setValue(configStrings[key].name, value);
}

void Config::sync()
{
    m_settings->sync();
}

Config::~Config() = default;
