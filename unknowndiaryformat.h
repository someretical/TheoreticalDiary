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

#ifndef UNKNOWNDIARYFORMAT_H
#define UNKNOWNDIARYFORMAT_H

#include <QDialog>

namespace Ui {
class UnknownDiaryFormat;
}

class UnknownDiaryFormat : public QDialog {
  Q_OBJECT

public:
  explicit UnknownDiaryFormat(QWidget *parent = nullptr);
  ~UnknownDiaryFormat();

public slots:
  void action_close();

private:
  Ui::UnknownDiaryFormat *ui;
};

#endif // UNKNOWNDIARYFORMAT_H
