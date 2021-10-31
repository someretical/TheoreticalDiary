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

#include "flushwindow.h"
#include "ui_flushwindow.h"

#include <QAction>

FlushWindow::FlushWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::FlushWindow) {
  ui->setupUi(this);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &FlushWindow::action_close,
          Qt::QueuedConnection);
}

FlushWindow::~FlushWindow() { delete ui; }

void FlushWindow::action_close() { accept(); }
