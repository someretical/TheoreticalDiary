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

#include "confirmoverwrite.h"
#include "mainwindow.h"
#include "theoreticaldiary.h"
#include "ui_confirmoverwrite.h"

#include <QAction>
#include <QFile>

ConfirmOverwrite::ConfirmOverwrite(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConfirmOverwrite) {
  ui->setupUi(this);

  QFile file(":/confirmoverwrite.qss");
  file.open(QIODevice::ReadOnly);
  QString str = file.readAll();
  file.close();
  setStyleSheet(str);

  auto action = findChild<QAction *>("action_no");
  addAction(action);
  connect(action, &QAction::triggered, this, &ConfirmOverwrite::reject,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_yes");
  addAction(action);
  connect(action, &QAction::triggered, this, &ConfirmOverwrite::accept,
          Qt::QueuedConnection);
}

ConfirmOverwrite::~ConfirmOverwrite() { delete ui; }
