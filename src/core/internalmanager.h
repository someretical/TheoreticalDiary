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

#ifndef INTERNAL_MANAGER_H
#define INTERNAL_MANAGER_H

#include "../util/eventfilters.h"

#include <QtWidgets>
#include <json.hpp>
#include <map>
#include <string>

namespace td {
enum class Rating { Unknown, VeryBad, Bad, Ok, Good, VeryGood };

struct Entry {
    bool important;
    Rating rating;
    std::string message;
    time_t last_updated;
};

inline void to_json(nlohmann::json &j, Entry const &e)
{
    j = nlohmann::json{
        {"important", e.important}, {"rating", e.rating}, {"message", e.message}, {"last_updated", e.last_updated}};
}

inline void from_json(nlohmann::json const &j, Entry &e)
{
    j.at("important").get_to<bool>(e.important);
    j.at("rating").get_to<Rating>(e.rating);
    j.at("message").get_to<std::string>(e.message);
    j.at("last_updated").get_to<time_t>(e.last_updated);
}

typedef std::map<int, Entry> MonthMap;
typedef std::map<int, MonthMap> YearMap;
typedef std::map<int, YearMap> DiaryLog;

struct Metadata {
    int version;
    time_t last_updated;
};

inline void to_json(nlohmann::json &j, Metadata const &m)
{
    j = nlohmann::json{{"version", m.version}, {"last_updated", m.last_updated}};
}

inline void from_json(nlohmann::json const &j, Metadata &m)
{
    j.at("version").get_to<int>(m.version);
    j.at("last_updated").get_to<time_t>(m.last_updated);
}

struct Settings {
};

inline void to_json(nlohmann::json & /* j */, Settings const & /* s */) {}

inline void from_json(nlohmann::json const & /* j */, Settings & /* s */) {}

struct Diary {
    DiaryLog log;
    Metadata metadata;
    Settings settings;
};

inline void to_json(nlohmann::json &j, Diary const &d)
{
    j = nlohmann::json{{"log", d.log}, {"metadata", d.metadata}, {"settings", d.settings}};
}

inline void from_json(nlohmann::json const &j, Diary &d)
{
    j.at("log").get_to<DiaryLog>(d.log);
    j.at("metadata").get_to<Metadata>(d.metadata);
    j.at("settings").get_to<Settings>(d.settings);
}

namespace OldVersions {
}

enum class GWrapperError { Auth, Network, IO, None, DriveFile };

struct Credentials {
    std::string access_token;
    std::string refresh_token;
};

inline void to_json(nlohmann::json &j, Credentials const &c)
{
    j = nlohmann::json{{"access_token", c.access_token}, {"refresh_token", c.refresh_token}};
}

inline void from_json(nlohmann::json const &j, Credentials &c)
{
    j.at("access_token").get_to<std::string>(c.access_token);
    j.at("refresh_token").get_to<std::string>(c.refresh_token);
}

enum class Theme { Light, Dark };

enum class Window { Main, Editor, Options };
} // namespace td

// The reason why this class exists is to redirect #include statements away from theoreticaldiary.h
// This is important as this class itself does not require theoreticaldiary.h to be included.
// This means that the number of circular references is greatly reduced meaning the structure of the whole program is
// simpler. Any files included to this one should not have any major files included with them!

/*
 * InternalManager class.
 */
class InternalManager : public QObject {
    Q_OBJECT

signals:
    void update_theme();
    void update_data(const QDate &new_date);

public:
    InternalManager();
    ~InternalManager();
    static InternalManager *instance();

    QString get_theme_str();
    td::Theme get_theme();
    QString data_location();
    void start_busy_mode(int const line, std::string const &func, std::string const &file);
    void end_busy_mode(int const line, std::string const &func, std::string const &file);
    void init_settings(bool const force_reset);

    QSettings *settings;
    InactiveFilter *inactive_filter;
    BusyFilter busy_filter;
    bool app_busy;
    bool internal_diary_changed;
    bool diary_file_changed;
};

#endif // INTERNAL_MANAGER_H
