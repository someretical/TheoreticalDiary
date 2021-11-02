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
#include <QThread>
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
  connect(this, &UpdatePassword::finished,
          [&]() { QApplication::restoreOverrideCursor(); });

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
  ui->alert_text->update();

  if (ui->first_password->text() != ui->second_password->text())
    return ui->alert_text->setText("The passwords do not match.");

  auto length = ui->first_password->text().length();

  if (257 < length)
    return ui->alert_text->setText("Passwords must be under 257 characters.");

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if (0 != length) {
    auto worker = new HashWorker();
    worker->moveToThread(&TheoreticalDiary::instance()->worker_thread);
    connect(&TheoreticalDiary::instance()->worker_thread, &QThread::finished,
            worker, &QObject::deleteLater);
    connect(worker, &HashWorker::done, this, &UpdatePassword::hash_set);
    connect(TheoreticalDiary::instance(), &TheoreticalDiary::sig_begin_hash,
            worker, &HashWorker::hash);
    TheoreticalDiary::instance()->worker_thread.start();

    ui->change_button->setEnabled(false);
    ui->cancel_button->setEnabled(false);
    emit TheoreticalDiary::instance()->sig_begin_hash(
        ui->first_password->text().toStdString());
  } else {
    TheoreticalDiary::instance()->encryptor->reset();
    hash_set();
  }
}

void UpdatePassword::hash_set() {
  TheoreticalDiary::instance()->changes_made();

  accept();
}

void UpdatePassword::closeEvent(QCloseEvent *event) {
  if (ui->change_button->isEnabled())
    event->accept();
}
