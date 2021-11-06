/*
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
#include "../core/theoreticaldiary.h"
#include "../util/zipper.h"
#include "mainwindow.h"
#include "ui_promptpassword.h"

#include <QDate>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaObject>
#include <QTimer>

PromptPassword::PromptPassword(const std::string &e, QWidget *parent)
    : QWidget(parent), ui(new Ui::PromptPassword) {
  ui->setupUi(this);
  ui->alert_text->setText("");

  // Pressing enter will try to decrypt
  enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
  connect(enter, &QShortcut::activated, ui->decrypt_button,
          [&]() { QMetaObject::invokeMethod(ui->decrypt_button, "clicked"); });

  // Make sure the line edit is in focus
  QTimer::singleShot(0, ui->password, [&]() { ui->password->setFocus(); });

  encrypted = new std::string(e);

  // The salt and IV are set here so they are only set ONCE to improve
  // performance.
  TheoreticalDiary::instance()->encryptor->set_salt(
      encrypted->substr(0, SALT_SIZE));
  encrypted->erase(0, SALT_SIZE);
  TheoreticalDiary::instance()->encryptor->set_decrypt_iv(
      encrypted->substr(0, IV_SIZE));
  encrypted->erase(0, IV_SIZE);

  connect(ui->show_button, &QPushButton::pressed, this,
          &PromptPassword::toggle_mask);
  connect(ui->show_button, &QPushButton::released, this,
          &PromptPassword::toggle_mask);
  connect(ui->cancel_button, &QPushButton::clicked,
          qobject_cast<MainWindow *>(parent), &MainWindow::show_main_menu);
  connect(ui->decrypt_button, &QPushButton::clicked, this,
          &PromptPassword::attempt_decrypt);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &PromptPassword::apply_theme);
  apply_theme();
}

PromptPassword::~PromptPassword() {
  delete ui;
  delete encrypted;
  delete enter;
}

void PromptPassword::apply_theme() {
  QFile file(":/global/passwords.qss");
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void PromptPassword::toggle_mask() {
  if (QLineEdit::Password == ui->password->echoMode()) {
    ui->password->setEchoMode(QLineEdit::Normal);
  } else {
    ui->password->setEchoMode(QLineEdit::Password);
  }
}

void PromptPassword::attempt_decrypt() {
  ui->alert_text->setText("Decrypting...");
  ui->alert_text->update();

  auto password = ui->password->text().toStdString();
  // I don't actually know what happens if a zero length string is passed to
  // set_key() so there's that :^)
  if (0 == password.size())
    return ui->alert_text->setText("Wrong password.");

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
  ui->cancel_button->setEnabled(false);
  // The signal must be emitted to ensure the thread is run properly
  emit TheoreticalDiary::instance()->sig_begin_hash(password);
}

void PromptPassword::retry() {
  QApplication::restoreOverrideCursor();
  ui->decrypt_button->setEnabled(true);
  ui->cancel_button->setEnabled(true);
}

void PromptPassword::hash_set() {

  auto res = TheoreticalDiary::instance()->encryptor->decrypt(*encrypted);
  if (!res) {
    ui->alert_text->setText("Wrong password.");
    return retry();
  }

  // Attempt the Gunzip the decrypted contents
  std::string decompressed;
  if (!Zipper::unzip(*res, decompressed) ||
      !TheoreticalDiary::instance()->diary_holder->load(decompressed)) {
    ui->alert_text->setText("");
    ui->alert_text->update();

    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);

    rip.setText("Parsing error.");
    rip.setInformativeText(
        "The app was unable to read the contents of the diary.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
    return retry();
  }

  ui->alert_text->setText("Decryption successful. Loading diary window...");
  QApplication::restoreOverrideCursor();
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_diary_menu(QDate::currentDate());
}
