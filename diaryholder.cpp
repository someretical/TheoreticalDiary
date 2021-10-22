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

#include "diaryholder.h"

#include <cryptlib.h>
#include <ctime>
#include <json.hpp>
#include <map>
#include <string>
#include <vector>

#define CURRENT_SAVE_VERSION 3

DiaryHolder::DiaryHolder() {
  diary = new td::Diary;
  key = new std::vector<CryptoPP::byte>();
}

DiaryHolder::~DiaryHolder() {
  delete diary;
  delete key;
}

void DiaryHolder::init() {
  td::Diary new_diary;
  new_diary.years = td::YearMap();
  new_diary.settings = td::Settings{false};
  new_diary.metadata = td::Metadata{CURRENT_SAVE_VERSION, std::time(nullptr)};

  *diary = new_diary;
}

void DiaryHolder::set_key(const std::vector<CryptoPP::byte> k) {
  key->clear();

  for (auto b : k)
    key->push_back(b);
}

bool DiaryHolder::load(std::string &raw) {
  auto json = nlohmann::json::parse(raw, nullptr, false);
  if (json.is_discarded())
    return false;

  try {
    *diary = json.get<td::Diary>();
  } catch (const nlohmann::json::exception &e) {
    return false;
  }

  return true;
}
