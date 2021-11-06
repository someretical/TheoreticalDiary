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

#include "diarystats.h"
#include "../core/theoreticaldiary.h"
#include "ui_diarystats.h"

DiaryStats::DiaryStats(QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryStats) {
  ui->setupUi(this);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryStats::apply_theme);
  apply_theme();
}

DiaryStats::~DiaryStats() { delete ui; }

void DiaryStats::apply_theme() {}
