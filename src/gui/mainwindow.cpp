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

#include "mainwindow.h"
#include "../core/theoreticaldiary.h"
#include "../util/zipper.h"
#include "diarymenu.h"
#include "mainmenu.h"
#include "promptpassword.h"
#include "ui_mainwindow.h"
#include "updatepassword.h"

#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <fstream>
#include <json.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle(QApplication::applicationName());

  previously_diary = false;
  previous_state = Qt::ApplicationActive;
  timer = new QTimer(this);
  timer->setTimerType(Qt::CoarseTimer);
  connect(timer, &QTimer::timeout, this, &MainWindow::inactive_time_up);

  QFile file(QString(":/%1/dangerbutton.qss")
                 .arg(TheoreticalDiary::instance()->theme()));
  file.open(QIODevice::ReadOnly);
  danger_button_style = new QString(file.readAll());
  file.close();

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &MainWindow::apply_theme);
  apply_theme();

  show_main_menu();
}

MainWindow::~MainWindow() {
  delete ui;
  delete danger_button_style;
  delete timer;
}

void MainWindow::focus_changed(const Qt::ApplicationState state) {
  if (Qt::ApplicationActive == state && previously_diary) {
    timer->stop();
  } else if (Qt::ApplicationInactive == state && previously_diary) {
    timer->start(300000); // 5 mins = 300000 ms
  }

  previous_state = state;
}

void MainWindow::inactive_time_up() {
  timer->stop();

  if (!previously_diary)
    return;

  emit sig_update_diary();
  save_diary(true); // Lock even if the save fails.

  // Clean up
  TheoreticalDiary::instance()->diary_modified = false;
  TheoreticalDiary::instance()->diary_holder->init();
  TheoreticalDiary::instance()->encryptor->reset();
  previously_diary = false;
  show_main_menu();
  update();

  //  QTimer::singleShot(0, [&]() {
  //    QMessageBox rip(this);
  //    QPushButton ok_button("OK", &rip);
  //    ok_button.setFlat(true);
  //    QFont f = ok_button.font();
  //    f.setPointSize(11);
  //    ok_button.setFont(f);

  //    rip.setText("Diary locked.");
  //    rip.setInformativeText(
  //        "The diary has been locked due to inactivity. If possible, any "
  //        "unsaved changes will have been saved.");
  //    rip.addButton(&ok_button, QMessageBox::AcceptRole);
  //    rip.setDefaultButton(&ok_button);
  //    rip.setTextInteractionFlags(Qt::NoTextInteraction);
  //    // Stop the locked dialog from putting the app back in focus.
  //    // This does not work for some reason :(
  //    rip.setAttribute(Qt::WA_ShowWithoutActivating);
  //    rip.exec();
  //  });
}

void MainWindow::apply_theme() {
  QFile file(QString(":/%1/material_cyan_dark.qss")
                 .arg(TheoreticalDiary::instance()->theme()));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void MainWindow::clear_grid() {
  QLayoutItem *child;
  while ((child = ui->central_widget->layout()->takeAt(0))) {
    child->widget()->deleteLater();
    delete child;
  }
}

void MainWindow::show_main_menu() {
  clear_grid();

  ui->central_widget->layout()->addWidget(new MainMenu(this));
}

void MainWindow::show_password_prompt(const std::string &e) {
  clear_grid();

  ui->central_widget->layout()->addWidget(new PromptPassword(e, this));
}

void MainWindow::show_update_password_prompt(const QDate &date) {
  clear_grid();

  ui->central_widget->layout()->addWidget(new UpdatePassword(date, this));
}

void MainWindow::show_diary_menu(const QDate &date) {
  clear_grid();

  previously_diary = true;
  ui->central_widget->layout()->addWidget(new DiaryMenu(date, this));
}

void MainWindow::save_error() {
  QMessageBox rip(this);
  QPushButton ok_button("OK", &rip);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  rip.setText("Save error.");
  rip.setInformativeText(
      "The app was unable to save the contents of the diary.");
  rip.addButton(&ok_button, QMessageBox::AcceptRole);
  rip.setDefaultButton(&ok_button);
  rip.setTextInteractionFlags(Qt::NoTextInteraction);

  rip.exec();
}

bool MainWindow::save_diary(const bool &ignore_errors) {
  std::string primary_path =
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat";
  std::string backup_path =
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat.bak";

  // Backup existing diary first
  std::ifstream src(primary_path, std::ios::binary);
  if (!src.fail()) {
    std::ofstream dst(backup_path, std::ios::binary);
    dst << src.rdbuf();
    dst.close();
  }
  src.close();

  // Update last_updated
  TheoreticalDiary::instance()->diary_holder->diary->metadata.last_updated =
      QDateTime::currentSecsSinceEpoch();
  nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);

  // Gzip JSON
  std::string compressed, encrypted;
  std::string decompressed = j.dump();
  Zipper::zip(compressed, decompressed);

  // Encrypt if there is a password set
  auto key_set = TheoreticalDiary::instance()->encryptor->key_set;
  if (key_set)
    TheoreticalDiary::instance()->encryptor->encrypt(compressed, encrypted);

  // Write to file
  std::ofstream ofs(primary_path, std::ios::binary);
  if (!ofs.fail()) {
    ofs << (key_set ? encrypted : compressed);
    ofs.close();

    TheoreticalDiary::instance()->diary_modified = false;
    return true;
  } else {
    if (!ignore_errors)
      save_error();

    return false;
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (previously_diary) {
    // If the previous window was the diary menu, then always exit to the main
    // menu.
    event->ignore();

    if (TheoreticalDiary::instance()->diary_modified) {
      QMessageBox confirm(this);

      QPushButton discard_button("Do not save", &confirm);
      QFont f = discard_button.font();
      f.setPointSize(11);
      discard_button.setFont(f);
      discard_button.setStyleSheet(*danger_button_style);
      QPushButton save_button("Save", &confirm);
      QFont f2 = save_button.font();
      f2.setPointSize(11);
      save_button.setFont(f2);
      QPushButton cancel_button("Cancel", &confirm);
      cancel_button.setFlat(true);
      QFont f3 = cancel_button.font();
      f3.setPointSize(11);
      cancel_button.setFont(f3);

      confirm.setText("There are unsaved changes.");
      confirm.setInformativeText(
          "Are you sure you want to quit without saving?");
      confirm.addButton(&save_button, QMessageBox::AcceptRole);
      confirm.addButton(&cancel_button, QMessageBox::RejectRole);
      confirm.addButton(&discard_button, QMessageBox::DestructiveRole);
      confirm.setDefaultButton(&save_button);
      confirm.setTextInteractionFlags(Qt::NoTextInteraction);

      switch (confirm.exec()) {
      case QMessageBox::AcceptRole:
        // If the diary failed to save, don't exit to the main menu.
        if (!save_diary(false))
          return;
        break;
      case QMessageBox::RejectRole:
        return;
      }
    }

    // Clean up
    TheoreticalDiary::instance()->diary_modified = false;
    TheoreticalDiary::instance()->diary_holder->init();
    TheoreticalDiary::instance()->encryptor->reset();
    previously_diary = false;
    return show_main_menu();
  }

  if (TheoreticalDiary::instance()->oauth_modified) {
    // TODO save auth credentials
  }

  if (TheoreticalDiary::instance()->local_settings_modified)
    TheoreticalDiary::instance()->save_settings();

  event->accept();
}
