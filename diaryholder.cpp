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
#include <string>
#include <vector>

DiaryHolder::DiaryHolder() {
  diary = new nlohmann::json;
  key = new std::vector<CryptoPP::byte>();
}

DiaryHolder::~DiaryHolder() {
  delete diary;
  delete key;
}

void DiaryHolder::init() {
  diary->clear();

  (*diary)["years"] = nlohmann::json::array();
  (*diary)["settings"] = nlohmann::json::object();
  (*diary)["metadata"] = nlohmann::json::object(
      {{"version", 1}, {"last_updated", std::time(nullptr)}});
}

void DiaryHolder::set_key(const std::vector<CryptoPP::byte> k) {
  key->clear();

  for (auto b : k)
    key->push_back(b);
}

bool DiaryHolder::validate(const nlohmann::json &json) {
  try {
    if (!json.contains("years") || !json.contains("settings") ||
        !json.contains("metadata"))
      return false;

    return true;
  } catch (const nlohmann::json::exception &e) {
    return false;
  }

  return false;
}

bool DiaryHolder::load(std::string &raw) {
  auto json = nlohmann::json::parse(raw, nullptr, false);
  if (json.is_discarded())
    return false;

  if (!DiaryHolder::validate(json))
    return false;

  diary->clear();
  diary->merge_patch(json);

  return true;
}
