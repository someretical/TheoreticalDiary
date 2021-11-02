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

#include "promptpassword.h"
#include "encryptor.h"
#include "mainwindow.h"
#include "theoreticaldiary.h"
#include "ui_promptpassword.h"
#include "zipper.h"

#include <QAction>
#include <QFile>
#include <cryptlib.h>

PromptPassword::PromptPassword(const std::string &e, std::string &d,
                               QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptPassword) {
  ui->setupUi(this);
  ui->wrong_password->setText("");

  encrypted = new std::string(e);
  // Pointer to a pointer to a string
  decrypted = new std::string *(&d);

  // The salt and IV are set here so they are only set ONCE every time the
  // password is prompted.
  TheoreticalDiary::instance()->encryptor->set_salt(
      encrypted->substr(0, SALT_SIZE));
  encrypted->erase(0, SALT_SIZE);
  TheoreticalDiary::instance()->encryptor->set_decrypt_iv(
      encrypted->substr(0, IV_SIZE));
  encrypted->erase(0, IV_SIZE);

  QFile file(":/promptpassword.qss");
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
  connect(this, &PromptPassword::finished,
          [&]() { QApplication::restoreOverrideCursor(); });

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
  ui->wrong_password->update();

  auto password = ui->password_box->text().toStdString();

  // I don't actually know what happens if a zero length string is passed to
  // set_key() so there's that :^)
  if (0 == password.size())
    return ui->wrong_password->setText("Wrong password.");

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  auto worker = new HashWorker();
  worker->moveToThread(&TheoreticalDiary::instance()->worker_thread);
  connect(&TheoreticalDiary::instance()->worker_thread, &QThread::finished,
          worker, &QObject::deleteLater);
  connect(worker, &HashWorker::done, this, &PromptPassword::hash_set);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::sig_begin_hash,
          worker, &HashWorker::hash);
  TheoreticalDiary::instance()->worker_thread.start();

  ui->decrypt_button->setEnabled(false);
  // The signal must be emitted to ensure the thread is run properly
  emit TheoreticalDiary::instance()->sig_begin_hash(password);
}

void PromptPassword::hash_set() {
  QApplication::restoreOverrideCursor();
  ui->decrypt_button->setEnabled(true);

  auto res = TheoreticalDiary::instance()->encryptor->decrypt(*encrypted);
  if (!res)
    return ui->wrong_password->setText("Wrong password.");

  **decrypted = *res;
  accept();
}
