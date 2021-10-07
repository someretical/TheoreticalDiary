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

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[])
    : QApplication(argc, argv) {
  gwrapper = new GoogleWrapper;
  diary_holder = new DiaryHolder;
  settings_provider = new SettingsProvider;

  unsaved_changes = false;

  QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
  if (!dir.exists())
    dir.mkpath(".");
}

TheoreticalDiary::~TheoreticalDiary() {
  delete gwrapper;
  delete diary_holder;
  delete settings_provider;
}

// static specifier is not needed here (if it was, it would cause a compiler
// error) see https://stackoverflow.com/a/31305772
TheoreticalDiary *TheoreticalDiary::instance() {
  return static_cast<TheoreticalDiary *>(QApplication::instance());
}

void TheoreticalDiary::changes_made() { unsaved_changes = true; }
