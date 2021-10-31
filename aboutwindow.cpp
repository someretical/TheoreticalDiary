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

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

#include <QAction>
#include <QFile>
#include <QString>

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);

  QFile file(":/aboutwindow.qss");
  file.open(QIODevice::ReadOnly);
  QString str = file.readAll();
  file.close();
  setStyleSheet(str);

  // Fill in contributors and licenses
  file.setFileName(":/LICENSES.txt");
  file.open(QIODevice::ReadOnly);
  str = file.readAll();
  file.close();
  ui->licenses_text->setPlainText(str);

  file.setFileName(":/CONTRIBUTORS.txt");
  file.open(QIODevice::ReadOnly);
  str = file.readAll();
  file.close();
  ui->contributors_text->setPlainText(str);

  file.setFileName(":/VERSION.txt");
  file.open(QIODevice::ReadOnly);
  str = file.readAll();
  file.close();
  ui->version->setText(str);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &AboutWindow::action_close,
          Qt::QueuedConnection);
}

AboutWindow::~AboutWindow() { delete ui; }

void AboutWindow::action_close() { accept(); }
