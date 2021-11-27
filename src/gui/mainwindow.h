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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

namespace Ui {
class MainWindow;
}

namespace td {
enum Window : int { Main, DiaryEditor, Options };
}

class MainWindow : public QMainWindow {
  Q_OBJECT

signals:
  void sig_update_diary();

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void clear_grid();
  void exit_diary_to_main_menu();
  void closeEvent(QCloseEvent *event);

  td::Window current_window;
  td::Window last_window;

  // Timer used to lock diary if window is left inactive for too long.
  QTimer *timer;
  Qt::ApplicationState previous_state;

public slots:
  void focus_changed(const Qt::ApplicationState state);
  void inactive_time_up();
  void apply_theme();
  void show_main_menu();
  void show_diary_menu(const QDate &date);
  void show_options_menu();
  bool save_diary(const bool ignore_errors);
  void diary_uploaded();

private:
  Ui::MainWindow *ui;

  void save_error();
  int confirm_exit_to_main_menu();
};

#endif // MAINWINDOW_H
