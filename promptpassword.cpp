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

#include "promptpassword.h"
#include "encryptor.h"
#include "mainwindow.h"
#include "theoreticaldiary.h"
#include "ui_promptpassword.h"
#include "zipper.h"

#include <QAction>
#include <QFile>
#include <cryptlib.h>
#include <string>

PromptPassword::PromptPassword(const std::string &e, std::string *d,
                               QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptPassword) {
  ui->setupUi(this);
  ui->wrong_password->setText("");

  encrypted = new std::string(e);
  // Pointer to a pointer to a string
  decrypted = new std::string *(d);

  QFile file(":/styles/promptpassword.qss");
  file.open(QIODevice::ReadOnly);
  QString str = file.readAll();
  file.close();
  setStyleSheet(str);

  auto action = findChild<QAction *>("action_decrypt");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::decrypt);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::reject,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_pwd");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::toggle_pwd);
}

PromptPassword::~PromptPassword() {
  delete ui;
  delete decrypted;
}

void PromptPassword::toggle_pwd() {
  if (ui->password_box->echoMode() == QLineEdit::Password) {
    ui->password_box->setEchoMode(QLineEdit::Normal);
  } else {
    ui->password_box->setEchoMode(QLineEdit::Password);
  }
}

void PromptPassword::decrypt() {
  ui->wrong_password->setText("");

  // Attempt to decrypt
  std::vector<CryptoPP::byte> hash;
  Encryptor::get_hash(ui->password_box->text().toStdString(), hash);

  std::string d;
  std::string copy = *encrypted;

  if (!Encryptor::decrypt(hash, copy, d))
    return ui->wrong_password->setText("Wrong password.");

  TheoreticalDiary::instance()->diary_holder->set_key(hash);
  **decrypted = d;

  accept();
}
