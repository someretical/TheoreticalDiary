/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2021  someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "../core/theoreticaldiary.h"
#include "../util/zipper.h"
#include "diarymenu.h"
#include "mainmenu.h"
#include "optionsmenu.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <json.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle(QApplication::applicationName());

  current_window = td::Window::Main;
  last_window = td::Window::Main;

  previous_state = Qt::ApplicationActive;
  timer = new QTimer(this);
  timer->setTimerType(Qt::CoarseTimer);
  connect(timer, &QTimer::timeout, this, &MainWindow::inactive_time_up,
          Qt::QueuedConnection);

  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_request_end, this, &MainWindow::diary_uploaded,
          Qt::QueuedConnection);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &MainWindow::apply_theme, Qt::QueuedConnection);
  apply_theme();

  show_main_menu();
}

MainWindow::~MainWindow() {
  delete ui;
  delete timer;
}

void MainWindow::diary_uploaded() {
  QApplication::restoreOverrideCursor();
  TheoreticalDiary::instance()->closeable = true;
  TheoreticalDiary::instance()->diary_file_modified = false;
}

void MainWindow::focus_changed(const Qt::ApplicationState state) {
  if ((Qt::ApplicationActive == state &&
       td::Window::DiaryEditor == current_window) ||
      (Qt::ApplicationActive == state &&
       td::Window::DiaryEditor == last_window &&
       td::Window::Options == current_window)) {
    timer->stop();
  } else if ((Qt::ApplicationInactive == state &&
              td::Window::DiaryEditor == current_window) ||
             (Qt::ApplicationInactive == state &&
              td::Window::DiaryEditor == last_window &&
              td::Window::Options == current_window)) {
    timer->start(300000); // 5 mins = 300000 ms
  }

  previous_state = state;
}

void MainWindow::exit_diary_to_main_menu() {
  // Clean up
  TheoreticalDiary::instance()->diary_modified = false;
  TheoreticalDiary::instance()->diary_holder->init();
  TheoreticalDiary::instance()->encryptor->reset();

  // Backup on Google Drive.
  if (TheoreticalDiary::instance()
          ->settings->value("sync_enabled", false)
          .toBool() &&
      TheoreticalDiary::instance()->diary_file_modified) {
    TheoreticalDiary::instance()->closeable = false;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    TheoreticalDiary::instance()->gwrapper->upload_diary(this, true);
  }

  show_main_menu();
}

void MainWindow::inactive_time_up() {
  timer->stop();

  if (td::Window::Main == current_window ||
      (td::Window::Options == current_window &&
       td::Window::Main == last_window))
    return;

  emit sig_update_diary();
  if (TheoreticalDiary::instance()->diary_modified)
    save_diary(true); // Passing true makes save_diary fail silently instead of
                      // triggering a popup dialog.

  exit_diary_to_main_menu();
}

void MainWindow::apply_theme() {
  QFile file(QString(":/%1/material_cyan_dark.qss")
                 .arg(TheoreticalDiary::instance()->theme()));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void MainWindow::clear_grid() {
  QWidget *w;

  // This code is so unbelievably janky, there is a ONE HUNDRED PERCENT chance
  // it will bite me back in the future. Basically, if there is some modal
  // widget like a dialog or message box that is open and is assigned on the
  // stack when clear_grid() is called, the application will seg fault because
  // Qt tries to double free the modal widget. The fix for this is to close all
  // active modal widgets before deleting the parent widget. NOTE THAT CLOSE !=
  // DELETE. IF THE MODAL WAS DYNAMICALLY CREATED, THE WA_DELETE_ON_CLOSE FLAG
  // NEEDS TO BE SET. I don't know if this will cause another seg fault though.
  // Also for some reason, the compiler wants me to enclose the assignment in
  // the while loop in another set of parentheses. :o
  while ((w = QApplication::activeModalWidget()))
    w->close();

  QLayoutItem *child;
  while ((child = ui->central_widget->layout()->takeAt(0))) {
    child->widget()->deleteLater();
    delete child;
  }
}

void MainWindow::show_main_menu() {
  last_window = current_window;
  current_window = td::Window::Main;

  clear_grid();
  ui->central_widget->layout()->addWidget(new MainMenu(this));
}

void MainWindow::show_options_menu() {
  const auto on_diary_editor = td::Window::DiaryEditor == current_window;
  last_window = current_window;
  current_window = td::Window::Options;

  clear_grid();
  ui->central_widget->layout()->addWidget(
      new OptionsMenu(on_diary_editor, this));
}

void MainWindow::show_diary_menu(const QDate &date) {
  last_window = current_window;
  current_window = td::Window::DiaryEditor;

  clear_grid();
  ui->central_widget->layout()->addWidget(new DiaryMenu(date, this));
}

void MainWindow::save_error() {
  QMessageBox rip(this);
  QPushButton ok_button("OK", &rip);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  rip.setFont(f);
  rip.setText("Save error.");
  rip.setInformativeText(
      "The app was unable to save the contents of the diary.");
  rip.addButton(&ok_button, QMessageBox::AcceptRole);
  rip.setDefaultButton(&ok_button);
  rip.setTextInteractionFlags(Qt::NoTextInteraction);

  rip.exec();
}

bool MainWindow::save_diary(const bool ignore_errors) {
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
  const nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);

  // Gzip JSON
  std::string compressed, encrypted;
  std::string decompressed = j.dump();
  Zipper::zip(compressed, decompressed);

  // Encrypt if there is a password set
  const auto key_set = TheoreticalDiary::instance()->encryptor->key_set;
  if (key_set)
    TheoreticalDiary::instance()->encryptor->encrypt(compressed, encrypted);

  // Write to file
  std::ofstream ofs(primary_path, std::ios::binary);
  if (!ofs.fail()) {
    ofs << (key_set ? encrypted : compressed);
    ofs.close();

    TheoreticalDiary::instance()->diary_modified = false;
    TheoreticalDiary::instance()->diary_file_changed();
    return true;
  } else {
    if (!ignore_errors)
      save_error();

    return false;
  }
}

int MainWindow::confirm_exit_to_main_menu() {
  QMessageBox confirm(this);

  QPushButton discard_button("Do not save", &confirm);
  QFont f = discard_button.font();
  f.setPointSize(11);
  discard_button.setFont(f);
  discard_button.setStyleSheet(
      *TheoreticalDiary::instance()->danger_button_style);
  QPushButton save_button("Save", &confirm);
  save_button.setFont(f);
  QPushButton cancel_button("Cancel", &confirm);
  cancel_button.setFlat(true);
  cancel_button.setFont(f);

  confirm.setFont(f);
  confirm.setText("There are unsaved changes.");
  confirm.setInformativeText("Are you sure you want to quit without saving?");
  confirm.addButton(&save_button, QMessageBox::AcceptRole);
  confirm.addButton(&cancel_button, QMessageBox::RejectRole);
  confirm.addButton(&discard_button, QMessageBox::DestructiveRole);
  confirm.setDefaultButton(&save_button);
  confirm.setTextInteractionFlags(Qt::NoTextInteraction);

  return confirm.exec();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (!TheoreticalDiary::instance()->closeable) {
    event->ignore();

  } else if (td::Window::Options == current_window &&
             td::Window::DiaryEditor == last_window) {
    event->ignore(); // Don't close the main window, but exit to the diary
                     // menu.
    show_diary_menu(QDate::currentDate());

  } else if (td::Window::Options == current_window &&
             td::Window::Main == last_window) {
    event->ignore(); // Don't close the main window, but exit to the main menu.
    show_main_menu();

  } else if (td::Window::DiaryEditor == current_window) {
    event->ignore(); // Don't close the main window, but exit to the main menu.

    if (TheoreticalDiary::instance()->diary_modified) {
      switch (confirm_exit_to_main_menu()) {
      case QMessageBox::AcceptRole:
        // If the diary failed to save, don't exit to the main menu.
        if (!save_diary(false))
          return;
        break;
        // Cancel button was pressed.
      case QMessageBox::RejectRole:
        return;
      }
    }

    exit_diary_to_main_menu();
  } else {
    event->accept(); // Actually exit the application.
  }
}
