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

  // Fill in contributors and licenses
  QString licenses;
  QString licenses_path(":/LICENSES.txt");
  QFile license_file(licenses_path);

  licenses = license_file.open(QIODevice::ReadOnly)
                 ? license_file.readAll()
                 : QString("Unable to load licenses.");
  license_file.close();

  QString contribs;
  QString contrib_path(":/CONTRIBUTORS.txt");
  QFile contrib_file(contrib_path);

  contribs = contrib_file.open(QIODevice::ReadOnly)
                 ? contrib_file.readAll()
                 : QString("Unable to load contributors.");
  contrib_file.close();

  ui->licenses_text->setPlainText(licenses);
  ui->contributors_text->setPlainText(contribs);

  QString version;
  QString version_path(":/VERSION.txt");
  QFile version_file(version_path);

  version = version_file.open(QIODevice::ReadOnly) ? version_file.readAll()
                                                   : QString("Unknown version");
  version_file.close();
  ui->version->setText(version);

  // Setup close action
  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &AboutWindow::action_close,
          Qt::QueuedConnection);
}

AboutWindow::~AboutWindow() { delete ui; }

void AboutWindow::action_close(bool b) { accept(); }
