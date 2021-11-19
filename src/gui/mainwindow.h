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

#include <QDate>
#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

signals:
  void sig_update_diary();

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void clear_grid();
  void closeEvent(QCloseEvent *event);

  // Tracks whether the previous window was the diary window.
  // Used to return to the main menu.
  bool previously_diary;

  // Timer used to lock diary if window is left inactive for too long.
  QTimer *timer;
  Qt::ApplicationState previous_state;

  // Cached stylesheets
  QString *danger_button_style;

public slots:
  void focus_changed(const Qt::ApplicationState state);
  void inactive_time_up();
  void apply_theme();
  void show_main_menu();
  void show_password_prompt(const std::string &e);
  void show_diary_menu(const QDate &date);
  void show_update_password_prompt(const QDate &date);
  bool save_diary(const bool &ignore_errors);

private:
  Ui::MainWindow *ui;

  void save_error();
};

#endif // MAINWINDOW_H
