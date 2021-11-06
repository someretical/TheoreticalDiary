/*
 * This file is part of Theoretical Diary.
 *
 * Theoretical Diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Theoretical Diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DIARYHOLDER_H
#define DIARYHOLDER_H

#include <QDate>
#include <cryptlib.h>
#include <json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace td {
enum Rating { Unknown, VeryBad, Bad, Ok, Good, VeryGood };

struct Entry {
  bool important;
  Rating rating;
  std::string message;
  time_t last_updated;
};

inline void to_json(nlohmann::json &j, const Entry &e) {
  j = nlohmann::json{{"important", e.important},
                     {"rating", e.rating},
                     {"message", e.message},
                     {"last_updated", e.last_updated}};
}

inline void from_json(const nlohmann::json &j, Entry &e) {
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

inline void to_json(nlohmann::json &j, const Metadata &m) {
  j = nlohmann::json{{"version", m.version}, {"last_updated", m.last_updated}};
}

inline void from_json(const nlohmann::json &j, Metadata &m) {
  j.at("version").get_to<int>(m.version);
  j.at("last_updated").get_to<time_t>(m.last_updated);
}

struct Settings {
  bool sync;
};

inline void to_json(nlohmann::json &j, const Settings &s) {
  j = nlohmann::json{{"sync", s.sync}};
}

inline void from_json(const nlohmann::json &j, Settings &s) {
  j.at("sync").get_to<bool>(s.sync);
}

struct Diary {
  DiaryLog log;
  Metadata metadata;
  Settings settings;
};

inline void to_json(nlohmann::json &j, const Diary &d) {
  j = nlohmann::json{
      {"log", d.log}, {"metadata", d.metadata}, {"settings", d.settings}};
}

inline void from_json(const nlohmann::json &j, Diary &d) {
  j.at("log").get_to<DiaryLog>(d.log);
  j.at("metadata").get_to<Metadata>(d.metadata);
  j.at("settings").get_to<Settings>(d.settings);
}

namespace OldVersions {}
} // namespace td

class DiaryHolder {
public:
  DiaryHolder();
  ~DiaryHolder();
  bool load(std::string &raw);
  void init();

  std::optional<td::DiaryLog::iterator> get_yearmap(const QDate &date);
  std::optional<td::YearMap::iterator> get_monthmap(const QDate &date);
  std::optional<td::MonthMap::iterator> get_entry(const QDate &date);
  void create_entry(const QDate &date, const td::Entry &entry);
  void delete_entry(const QDate &date);

  td::Diary *diary;
};

#endif // DIARYHOLDER_H
