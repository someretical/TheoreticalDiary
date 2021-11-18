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

#ifndef CALENDARBUTTON_H
#define CALENDARBUTTON_H

class DiaryEditor;

#include "../core/diaryholder.h"
#include "diaryeditor.h"

#include <QPushButton>
#include <optional>

namespace td {
struct CalendarButtonData {
  std::optional<int> day;
  std::optional<DiaryEditor *> parent;
  std::optional<bool> important;
  std::optional<td::Rating> rating;
  std::optional<bool> selected;
};
} // namespace td

class CalendarButton : public QPushButton {
  Q_OBJECT

signals:
  void sig_clicked(const int day);

public:
  explicit CalendarButton(const td::CalendarButtonData &d);
  ~CalendarButton();

  td::CalendarButtonData data;

public slots:
  void clicked_on();
  void re_render(const td::CalendarButtonData &d);
};

#endif // CALENDARBUTTON_H
