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

#include "nodiaryfound.h"
#include "ui_nodiaryfound.h"

#include <QAction>
#include <QFile>

NoDiaryFound::NoDiaryFound(QWidget *parent)
    : QDialog(parent), ui(new Ui::NoDiaryFound) {
  ui->setupUi(this);

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &NoDiaryFound::action_close,
          Qt::QueuedConnection);
}

NoDiaryFound::~NoDiaryFound() { delete ui; }

void NoDiaryFound::action_close() { accept(); }
