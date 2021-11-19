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

#ifndef DIARYMENU_H
#define DIARYMENU_H

#include <QDate>
#include <QShortcut>
#include <QWidget>

namespace Ui {
class DiaryMenu;
}

class DiaryMenu : public QWidget {
  Q_OBJECT

public:
  explicit DiaryMenu(const QDate &date, QWidget *parent = nullptr);
  ~DiaryMenu();

  static QString get_day_suffix(const int &day);

  QDate *first_created;

public slots:
  void apply_theme();
  void close_window();
  void change_password();
  void export_diary();
  void toggle_sync();
  void tab_changed(const int &tab);

private:
  Ui::DiaryMenu *ui;
  QShortcut *save_shortcut;
};

#endif // DIARYMENU_H
