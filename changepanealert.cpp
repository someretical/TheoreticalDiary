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

#include "changepanealert.h"
#include "theoreticaldiary.h"
#include "ui_changepanealert.h"

#include <QAction>
#include <QFile>

ChangePaneAlert::ChangePaneAlert(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChangePaneAlert) {
  ui->setupUi(this);

  QFile ss_file(":/styles/confirmoverwrite.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  auto action = findChild<QAction *>("action_no");
  addAction(action);
  connect(action, &QAction::triggered, this, &ChangePaneAlert::reject,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_yes");
  addAction(action);
  connect(action, &QAction::triggered, this, &ChangePaneAlert::accept,
          Qt::QueuedConnection);
}

ChangePaneAlert::~ChangePaneAlert() { delete ui; }
