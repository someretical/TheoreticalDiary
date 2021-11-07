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

#ifndef DIARYPIXELS_H
#define DIARYPIXELS_H

#include "diaryeditor.h"

#include <QLabel>
#include <QWidget>
#include <vector>

namespace Ui {
class DiaryPixels;
}

class DiaryPixels : public QWidget {
  Q_OBJECT

public:
  explicit DiaryPixels(const DiaryEditor *editor, QWidget *parent = nullptr);
  ~DiaryPixels();

  QDate *current_year;
  std::vector<QString> *rating_stylesheets;

public slots:
  void apply_theme();
  void render_grid();

private:
  Ui::DiaryPixels *ui;
};

class PixelLabel : public QLabel {
  Q_OBJECT

public:
  explicit PixelLabel(const td::Rating &r, QWidget *parent = nullptr);
  ~PixelLabel();
};

#endif // DIARYPIXELS_H
