/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DIARYHOLDER_H
#define DIARYHOLDER_H

#include <cryptlib.h>
#include <json.hpp>
#include <string>
#include <vector>

namespace td {
enum Rating { VeryBad = 1, Bad, Ok, Good, VeryGood };

enum Month {
  January = 1,
  February,
  March,
  April,
  May,
  June,
  July,
  August,
  September,
  October,
  November,
  December
};

struct Entry {
  int day;
  Rating rating;
  std::string message;
};

inline void to_json(nlohmann::json &j, const Entry &e) {
  j = nlohmann::json{
      {"day", e.day}, {"rating", e.rating}, {"message", e.message}};
}

inline void from_json(const nlohmann::json &j, Entry &e) {
  j.at("day").get_to<int>(e.day);
  j.at("rating").get_to<Rating>(e.rating);
  j.at("message").get_to<std::string>(e.message);
}

typedef std::map<Month, Entry> EntryMap;

struct MonthContainer {
  Month month;
  EntryMap days;
};

inline void to_json(nlohmann::json &j, const MonthContainer &m) {
  j = nlohmann::json{{"month", m.month}, {"days", m.days}};
}

inline void from_json(const nlohmann::json &j, MonthContainer &m) {
  j.at("month").get_to<Month>(m.month);
  j.at("days").get_to<EntryMap>(m.days);
}

typedef std::map<int, MonthContainer> MonthMap;

struct YearContainer {
  int year;
  MonthMap months;
};

inline void to_json(nlohmann::json &j, const YearContainer &y) {
  j = nlohmann::json{{"year", y.year}, {"months", y.months}};
}

inline void from_json(const nlohmann::json &j, YearContainer &y) {
  j.at("year").get_to<int>(y.year);
  j.at("months").get_to<MonthMap>(y.months);
}

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

struct Settings {};

inline void to_json(nlohmann::json &j, const Settings &s) {
  j = nlohmann::json{};
}

inline void from_json(const nlohmann::json &j, Settings &s) {}

typedef std::map<int, YearContainer> YearMap;

struct Diary {
  YearMap years;
  Metadata metadata;
  Settings settings;
};

inline void to_json(nlohmann::json &j, const Diary &d) {
  j = nlohmann::json{
      {"years", d.years}, {"metadata", d.metadata}, {"settings", d.settings}};
}

inline void from_json(const nlohmann::json &j, Diary &d) {
  j.at("years").get_to<YearMap>(d.years);
  j.at("metadata").get_to<Metadata>(d.metadata);
  j.at("settings").get_to<Settings>(d.settings);
}
} // namespace td

class DiaryHolder {
public:
  DiaryHolder();
  ~DiaryHolder();
  bool load(std::string &raw);
  void init();
  void set_key(const std::vector<CryptoPP::byte> k);

  td::Diary *diary;
  std::vector<CryptoPP::byte> *key;
};

#endif // DIARYHOLDER_H
