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

#include "settingsprovider.h"
#include "theoreticaldiary.h"

#include <QStandardPaths>
#include <fstream>
#include <json.hpp>
#include <string>

SettingsProvider::SettingsProvider() {
  bak1_id = new std::string("");
  bak2_id = new std::string("");
  sync_enabled = new bool(false);

  load();
}

SettingsProvider::~SettingsProvider() {
  delete bak1_id;
  delete bak2_id;
  delete sync_enabled;
}

void SettingsProvider::init() {
  bak1_id->clear();
  bak2_id->clear();
  *sync_enabled = false;

  TheoreticalDiary::instance()->changes_made();
}

void SettingsProvider::load() {
  std::ifstream ifs(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/settings.json");
  if (ifs.fail())
    return init();

  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  auto json = nlohmann::json::parse(content, nullptr, false);
  if (json.is_discarded())
    return init();

  try {
    bak1_id->assign(json.at("bak1_id"));
    bak2_id->assign(json.at("bak2_id"));
    *sync_enabled = json.at("sync_enabled");

  } catch (const nlohmann::json::exception &e) {
    return init();
  }
}

bool SettingsProvider::save() {
  nlohmann::json json;
  json["bak1_id"] = *bak1_id;
  json["bak2_id"] = *bak2_id;
  json["sync_enabled"] = *sync_enabled;

  std::ofstream ifs(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/settings.json");

  if (ifs.fail())
    return false;

  ifs << json.dump();
  ifs.close();

  return true;
}
