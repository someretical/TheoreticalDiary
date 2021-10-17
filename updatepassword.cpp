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

  QFile ss_file(":/styles/updatepassword.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

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

  if (ui->first_password->text() != ui->second_password->text()) {
    return ui->alert_text->setText("The passwords do not match.");
  }

  if (ui->first_password->text().size() == 0) {
    TheoreticalDiary::instance()->diary_holder->key->clear();
  } else {
    std::vector<CryptoPP::byte> hash;
    Encryptor::get_hash(ui->first_password->text().toStdString(), hash);
    TheoreticalDiary::instance()->diary_holder->set_key(hash);
  }

  TheoreticalDiary::instance()->changes_made();

  accept();
}
