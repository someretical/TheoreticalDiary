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

#include "updatepassword.h"
#include "encryptor.h"
#include "theoreticaldiary.h"
#include "ui_updatepassword.h"

#include <QAction>
#include <QFile>
#include <cryptlib.h>
#include <vector>

UpdatePassword::UpdatePassword(QWidget *parent)
    : QDialog(parent), ui(new Ui::UpdatePassword) {
  ui->setupUi(this);
  ui->alert_text->setText("");

  QFile file(":/updatepassword.qss");
  file.open(QIODevice::ReadOnly);
  QString str = file.readAll();
  file.close();
  setStyleSheet(str);

  auto action = findChild<QAction *>("action_change");
  addAction(action);
  connect(action, &QAction::triggered, this, &UpdatePassword::attempt_change);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &UpdatePassword::action_close);

  action = findChild<QAction *>("action_toggle_password");
  addAction(action);
  connect(action, &QAction::triggered, this, &UpdatePassword::toggle_password);
}

UpdatePassword::~UpdatePassword() { delete ui; }

void UpdatePassword::toggle_password() {
  if (ui->first_password->echoMode() == QLineEdit::Password) {
    ui->first_password->setEchoMode(QLineEdit::Normal);
    ui->second_password->setEchoMode(QLineEdit::Normal);
  } else {
    ui->first_password->setEchoMode(QLineEdit::Password);
    ui->second_password->setEchoMode(QLineEdit::Password);
  }
}

void UpdatePassword::action_close() { accept(); }

void UpdatePassword::attempt_change() {
  ui->alert_text->setText("");
  QCoreApplication::processEvents();

  if (ui->first_password->text() != ui->second_password->text())
    return ui->alert_text->setText("The passwords do not match.");

  auto length = ui->first_password->text().length();

  if (1 == length)
    return ui->alert_text->setText(
        "Passwords need to be 0 or >= 2 characters long.");

  if (32 < length)
    return ui->alert_text->setText("Passwords must be <= 32 characters.");

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  TheoreticalDiary::instance()->encryptor->regenerate_salt();
  TheoreticalDiary::instance()->encryptor->set_key(
      0 == length ? "a" : ui->first_password->text().toStdString());
  TheoreticalDiary::instance()->changes_made();
  QApplication::restoreOverrideCursor();

  accept();
}
