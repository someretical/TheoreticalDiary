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

#include "theoreticaldiary.h"

#include <QStandardPaths>
#include <fstream>
#include <json.hpp>
#include <string>

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[])
    : QApplication(argc, argv) {
  gwrapper = new GoogleWrapper;
  diary_holder = new DiaryHolder;
  local_settings = new td::LocalSettings{"", "", false};
  unsaved_changes = new bool(false);

  connect(gwrapper, &GoogleWrapper::sig_token_changed, this,
          &TheoreticalDiary::changes_made);
}

TheoreticalDiary::~TheoreticalDiary() {
  delete gwrapper;
  delete diary_holder;
  delete local_settings;
  delete unsaved_changes;
}

// static specifier is not needed here (if it was, it would cause a compiler
// error) see https://stackoverflow.com/a/31305772
TheoreticalDiary *TheoreticalDiary::instance() {
  return static_cast<TheoreticalDiary *>(QApplication::instance());
}

void TheoreticalDiary::changes_made() { *unsaved_changes = true; }

void TheoreticalDiary::load_settings() {
  std::ifstream ifs(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/settings.json");
  if (ifs.fail())
    return;

  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  auto json = nlohmann::json::parse(content, nullptr, false);
  if (json.is_discarded())
    return;

  *local_settings = json.get<td::LocalSettings>();
}

bool TheoreticalDiary::save_settings() {
  nlohmann::json stringified = *local_settings;

  std::ofstream ifs(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/settings.json");

  if (ifs.fail())
    return false;

  ifs << stringified.dump();
  ifs.close();

  return true;
}
