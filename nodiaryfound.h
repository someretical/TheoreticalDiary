/**
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

#ifndef NODIARYFOUND_H
#define NODIARYFOUND_H

#include <QDialog>

namespace Ui {
class NoDiaryFound;
}

class NoDiaryFound : public QDialog {
  Q_OBJECT

public:
  explicit NoDiaryFound(QWidget *parent = nullptr);
  ~NoDiaryFound();

public slots:
  void action_close();

private:
  Ui::NoDiaryFound *ui;
};

#endif // NODIARYFOUND_H
