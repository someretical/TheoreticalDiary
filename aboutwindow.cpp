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

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  // Fill in contributors and licenses
  QFile license_file(":/text/LICENSES.txt");
  license_file.open(QIODevice::ReadOnly);
  QString licenses = license_file.readAll();
  license_file.close();

  QFile contrib_file(":/text/CONTRIBUTORS.txt");
  contrib_file.open(QIODevice::ReadOnly);
  QString contribs = contrib_file.readAll();
  contrib_file.close();

  ui->licenses_text->setPlainText(licenses);
  ui->contributors_text->setPlainText(contribs);

  QFile version_file(":/text/VERSION.txt");
  version_file.open(QIODevice::ReadOnly);
  QString version = version_file.readAll();
  version_file.close();
  ui->version->setText(version);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &AboutWindow::action_close,
          Qt::QueuedConnection);
}

AboutWindow::~AboutWindow() { delete ui; }

void AboutWindow::action_close() { accept(); }
