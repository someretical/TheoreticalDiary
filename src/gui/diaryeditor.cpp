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

#include "diaryeditor.h"
#include "../core/theoreticaldiary.h"
#include "ui_diaryeditor.h"

DiaryEditor::DiaryEditor(QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryEditor) {
  ui->setupUi(this);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryEditor::apply_theme);
  apply_theme();
}

DiaryEditor::~DiaryEditor() { delete ui; }

void DiaryEditor::apply_theme() {
  //  QFile file(":/" + TheoreticalDiary::instance()->theme() +
  //  "/diarymenu.qss"); file.open(QIODevice::ReadOnly);
  //  setStyleSheet(file.readAll());
  //  file.close();
}
