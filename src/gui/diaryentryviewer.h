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

#ifndef DIARYENTRYVIEWER_H
#define DIARYENTRYVIEWER_H

#include <QWidget>

namespace Ui {
class DiaryEntryViewer;
}

class DiaryEntryViewer : public QWidget {
  Q_OBJECT

public:
  explicit DiaryEntryViewer(QWidget *parent = nullptr);
  ~DiaryEntryViewer();

public slots:
  void apply_theme();

private:
  Ui::DiaryEntryViewer *ui;
};

#endif // DIARYENTRYVIEWER_H
