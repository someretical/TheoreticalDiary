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

#include "diaryholder.h"

#include <QDateTime>
#include <map>

const int CURRENT_SAVE_VERSION = 4;

DiaryHolder::DiaryHolder() { diary = new td::Diary; }

DiaryHolder::~DiaryHolder() { delete diary; }

void DiaryHolder::init() {
  *diary = td::Diary{
      td::DiaryLog(),
      td::Metadata{CURRENT_SAVE_VERSION, QDateTime::currentSecsSinceEpoch()},
      td::Settings{false}};
}

bool DiaryHolder::load(std::string &raw) {
  auto json = nlohmann::json::parse(raw, nullptr, false);
  if (json.is_discarded())
    return false;

  // Here is where updating save versions will occur *if* I every actually get
  // around to doing this which will probably never happen because I'm the only
  // person using my own app :')

  try {
    *diary = json.get<td::Diary>();
  } catch (const nlohmann::json::exception &e) {
    return false;
  }

  return true;
}
