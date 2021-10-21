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

#include <QAction>
#include <QFile>
#include <cryptlib.h>
#include <string>

PromptPassword::PromptPassword(const std::string &uncompressed, QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptPassword) {
  ui->setupUi(this);
  ui->wrong_password->setText("");

  to_be_decrypted = new std::string(uncompressed);

  QFile ss_file(":/styles/promptpassword.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();

  setStyleSheet(stylesheet);

  auto action = findChild<QAction *>("action_decrypt");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::decrypt);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::close_window,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_pwd");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptPassword::toggle_pwd);

  connect(this, &PromptPassword::sig_complete,
          qobject_cast<MainWindow *>(parent), &MainWindow::prompt_pwd_callback);
}

PromptPassword::~PromptPassword() {
  delete ui;
  delete to_be_decrypted;
}

void PromptPassword::close_window() { accept(); }

void PromptPassword::toggle_pwd() {
  if (ui->password_box->echoMode() == QLineEdit::Password) {
    ui->password_box->setEchoMode(QLineEdit::Normal);
  } else {
    ui->password_box->setEchoMode(QLineEdit::Password);
  }
}

void PromptPassword::decrypt() {
  ui->wrong_password->setText("");

  std::vector<CryptoPP::byte> hash;
  Encryptor::get_hash(ui->password_box->text().toStdString(), hash);

  std::string decrypted;
  std::string copy(*to_be_decrypted);
  auto success = Encryptor::decrypt(hash, copy, decrypted);

  if (!success) {
    return ui->wrong_password->setText("Wrong password.");
  }

  success = TheoreticalDiary::instance()->diary_holder->load(decrypted);

  if (!success) {
    emit sig_complete(td::Res::No);
    accept();
  }

  TheoreticalDiary::instance()->diary_holder->set_key(hash);
  emit sig_complete(td::Res::Yes);
  accept();
}
