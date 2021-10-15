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

#ifndef DIARYWINDOW_H
#define DIARYWINDOW_H

#include <QDialog>

namespace Ui {
class DiaryWindow;
}

class DiaryWindow : public QDialog {
  Q_OBJECT

public:
  explicit DiaryWindow(QWidget *parent = nullptr);
  ~DiaryWindow();

public slots:
  void reject();
  void action_save();
  void update_password();

  void confirm_close_callback(const int code);

private:
  Ui::DiaryWindow *ui;
};

#endif // DIARYWINDOW_H
