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

#ifndef MAINMENU_H
#define MAINMENU_H

#include "../core/theoreticaldiary.h"

#include <QShortcut>
#include <QWidget>
#include <string>

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget {
  Q_OBJECT

public:
  explicit MainMenu(QWidget *parent = nullptr);
  ~MainMenu();

public slots:
  void apply_theme();
  void decrypt_diary();
  void new_diary();
  void import_diary();
  void open_options();
  bool get_diary_contents();
  void decrypt_diary_cb(bool do_decrypt = true);

private:
  Ui::MainMenu *ui;
  QShortcut *enter;
};

#endif // MAINMENU_H
