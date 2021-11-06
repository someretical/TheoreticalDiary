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

#include "updatepassword.h"
#include "../core/theoreticaldiary.h"
#include "mainwindow.h"
#include "ui_updatepassword.h"

#include <QShortcut>

UpdatePassword::UpdatePassword(const QDate &date, QWidget *parent)
    : QWidget(parent), ui(new Ui::UpdatePassword) {
  ui->setupUi(this);
  ui->alert_text->setText("");

  return_date = new QDate(date);

  enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
  connect(enter, &QShortcut::activated, ui->change_button,
          [&]() { QMetaObject::invokeMethod(ui->change_button, "clicked"); });
  QTimer::singleShot(0, ui->password, [&]() { ui->password->setFocus(); });

  connect(ui->show_button, &QPushButton::pressed, this,
          &UpdatePassword::toggle_mask);
  connect(ui->show_button, &QPushButton::released, this,
          &UpdatePassword::toggle_mask);
  connect(ui->cancel_button, &QPushButton::clicked, [&]() {
    qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->show_diary_menu(*return_date);
  });
  connect(ui->change_button, &QPushButton::clicked, this,
          &UpdatePassword::attempt_change);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &UpdatePassword::apply_theme);
  apply_theme();
}

UpdatePassword::~UpdatePassword() {
  delete ui;
  delete return_date;
  delete enter;
}

void UpdatePassword::apply_theme() {
  QFile file(":/global/passwords.qss");
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void UpdatePassword::toggle_mask() {
  if (QLineEdit::Password == ui->password->echoMode()) {
    ui->password->setEchoMode(QLineEdit::Normal);
    ui->password2->setEchoMode(QLineEdit::Normal);
  } else {
    ui->password->setEchoMode(QLineEdit::Password);
    ui->password2->setEchoMode(QLineEdit::Password);
  }
}

void UpdatePassword::hash_set() {
  TheoreticalDiary::instance()->diary_changed();
  QApplication::restoreOverrideCursor();

  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_diary_menu(*return_date);
}

void UpdatePassword::attempt_change() {
  ui->alert_text->setText("");
  ui->alert_text->update();

  auto password = ui->password->text();
  if (password != ui->password2->text())
    return ui->alert_text->setText("The passwords do not match.");

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if (0 != password.length()) {
    ui->alert_text->setText("Changing password...");

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

    TheoreticalDiary::instance()->encryptor->regenerate_salt();
    emit TheoreticalDiary::instance()->sig_begin_hash(password.toStdString());
  } else {
    TheoreticalDiary::instance()->encryptor->reset();
    hash_set();
  }
}
