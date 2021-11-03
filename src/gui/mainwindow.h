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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void clear_grid();
  void closeEvent(QCloseEvent *event);

  // Tracks whether the previous window was the diary window.
  // Used to return to the main menu.
  bool previously_diary;

  // Cached stylesheets
  QString *danger_button_style;

public slots:
  void apply_theme();
  void show_main_menu();
  void show_password_prompt(const std::string &e);
  void show_diary_menu(const QDate &date);
  void show_update_password_prompt(const QDate &date);
  bool save_diary();

private:
  Ui::MainWindow *ui;

  void save_error();
};

#endif // MAINWINDOW_H
