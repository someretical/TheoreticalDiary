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

#ifndef UPDATEPASSWORD_H
#define UPDATEPASSWORD_H

#include <QDate>
#include <QShortcut>
#include <QWidget>

namespace Ui {
class UpdatePassword;
}

class UpdatePassword : public QWidget {
  Q_OBJECT

public:
  explicit UpdatePassword(const QDate &date, QWidget *parent = nullptr);
  ~UpdatePassword();

public slots:
  void apply_theme();
  void attempt_change();
  void hash_set();
  void toggle_mask();

private:
  QDate *return_date;
  Ui::UpdatePassword *ui;
  QShortcut *enter;
};

#endif // UPDATEPASSWORD_H
