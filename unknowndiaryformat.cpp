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

#include "unknowndiaryformat.h"
#include "ui_unknowndiaryformat.h"

#include <QAction>

UnknownDiaryFormat::UnknownDiaryFormat(QWidget *parent)
    : QDialog(parent), ui(new Ui::UnknownDiaryFormat) {
  ui->setupUi(this);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &UnknownDiaryFormat::action_close,
          Qt::QueuedConnection);
}

UnknownDiaryFormat::~UnknownDiaryFormat() { delete ui; }

void UnknownDiaryFormat::action_close() { accept(); }
