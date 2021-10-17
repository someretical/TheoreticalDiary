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

#include "unsavedchanges.h"
#include "ui_unsavedchanges.h"

#include <QAction>

UnsavedChangesBase::UnsavedChangesBase(QWidget *parent)
    : QDialog(parent), ui(new Ui::UnsavedChangesBase) {
  ui->setupUi(this);

  auto action = findChild<QAction *>("action_no");
  addAction(action);
  connect(action, &QAction::triggered, this, &UnsavedChangesBase::action_no,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_yes");
  addAction(action);
  connect(action, &QAction::triggered, this, &UnsavedChangesBase::action_yes,
          Qt::QueuedConnection);
}

UnsavedChangesBase::~UnsavedChangesBase() { delete ui; }

void UnsavedChangesBase::action_no() {
  emit sig_complete(1);
  accept();
}

void UnsavedChangesBase::action_yes() {
  emit sig_complete(0);
  accept();
}
