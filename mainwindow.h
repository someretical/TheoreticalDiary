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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "googlewrapper.h"

#include <QCloseEvent>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void closeEvent(QCloseEvent *event);
  void create_new_diary();

private:
  Ui::MainWindow *ui;

public slots:
  void open_diary();
  void new_diary();
  void dl_diary();
  void import_diary();
  void flush_credentials();
  void dump_drive();
  void about_app();
  void toggle_advanced_options();
  void quit_app();
  void real_import_diary();

  void prompt_pwd_callback(const int code);
  void confirm_overwrite_callback(const int code);
  void import_diary_callback(const int code);
  void oauth2_callback(const int code);
};
#endif // MAINWINDOW_H
