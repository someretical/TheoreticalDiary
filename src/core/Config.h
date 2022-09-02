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

#ifndef THEORETICAL_DIARY_CONFIG_H
#define THEORETICAL_DIARY_CONFIG_H

#include <QSettings>

class Config : public QObject {
    Q_OBJECT

public:
    enum PieChartSortType {
        PieChart_Rating = 1,
        PieChart_Days = 2,
    };

    enum SortOrder { AscendingOrder = 0, DescendingOrder = 1 };

    enum ConfigKey {
        RecentDiaries,
        MaximumRecentDiaries,

        Diary_LockTimeoutEnabled,
        Diary_LockTimeout,
        Diary_SaveOnLock,
        Diary_BackupBeforeSave,

        PieChartSortType,
        PieChartSortOrder,

        GUI_Theme,
        GUI_MainWindowState,
        GUI_MainWindowGeometry,
        GUI_LastSessionDiaries,
        GUI_LastDiaryTabIndex,
    };

    explicit Config(QObject *parent);
    ~Config() override;

    static auto instance() -> Config *;
    static auto getDefault(ConfigKey key) -> QVariant;
    auto get(ConfigKey key) -> QVariant;
    void set(ConfigKey key, const QVariant &value);
    void sync();

private:
    static Config *m_instance;
    QSettings *m_settings;
};

inline auto config() -> Config *
{
    return Config::instance();
}

#endif // THEORETICAL_DIARY_CONFIG_H
