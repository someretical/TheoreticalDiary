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
  key = new std::vector<CryptoPP::byte>;
}

DiaryHolder::~DiaryHolder() {
  delete diary;
  delete key;
}

void DiaryHolder::init() {
  key->clear();
  *diary = td::Diary{td::YearMap(),
                     td::Metadata{CURRENT_SAVE_VERSION, std::time(nullptr)},
                     td::Settings{false}};
}

void DiaryHolder::set_key(const std::vector<CryptoPP::byte> k) {
  key->assign(k.begin(), k.end());
}

bool DiaryHolder::load(std::string &raw) {
  auto json = nlohmann::json::parse(raw, nullptr, false);
  if (json.is_discarded())
    return false;

  // Here is where updating save versions will occur

  try {
    *diary = json.get<td::Diary>();
  } catch (const nlohmann::json::exception &e) {
    return false;
  }

  return true;
}
