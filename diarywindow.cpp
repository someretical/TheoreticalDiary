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

#include "diarywindow.h"
#include "diaryholder.h"
#include "encryptor.h"
#include "saveerror.h"
#include "theoreticalcalendar.h"
#include "theoreticaldiary.h"
#include "ui_diarywindow.h"
#include "unsavedchanges.h"
#include "updatepassword.h"
#include "zipper.h"

#include <QAction>
#include <QDate>
#include <QString>
#include <ctime>
#include <fstream>
#include <string>

// https://stackoverflow.com/a/9130114
std::string DiaryWindow::get_day_suffix(const int day) {
  switch (day) {
  case 1:
  case 21:
  case 31:
    return "st";
  case 2:
  case 22:
    return "nd";
  case 3:
  case 23:
    return "rd";
  default:
    return "th";
  }
}

DiaryWindow::DiaryWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::DiaryWindow) {
  ui->setupUi(this);

  previous_selection = new int[3]{-1, -1, -1};

  QFile ss_file(":/styles/diarywindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  // For some reason, calling this->setStyleSheet() has no effect on the
  // contents of the tab widget. So we have to call it on the tab widget
  // specifically.
  ui->editor->setStyleSheet(stylesheet);

  calendar = new TheoreticalCalendar(this);
  ui->calender_box->addWidget(calendar, Qt::AlignHCenter | Qt::AlignTop);

  ui->status_text->setText("");

  auto action = findChild<QAction *>("action_save");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::action_save);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::reject);

  action = findChild<QAction *>("action_update_password");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::update_password);
}

DiaryWindow::~DiaryWindow() {
  delete ui;
  delete calendar;
  delete[] previous_selection;
}

void DiaryWindow::update_info(const int year, const int month, const int day) {
  if (year == previous_selection[0] && month == previous_selection[1] &&
      day == previous_selection[2])
    return;

  previous_selection[0] = year;
  previous_selection[1] = month;
  previous_selection[2] = day;

  QDate new_date(year, month, day);

  ui->date_placeholder->setText(
      QString::number(day) +
      QString::fromStdString(DiaryWindow::get_day_suffix(day)) +
      new_date.toString(" MMMM yyyy"));
}

void DiaryWindow::reject() {
  if (*(TheoreticalDiary::instance()->unsaved_changes)) {
    UnsavedChanges<DiaryWindow> w(&DiaryWindow::confirm_close_callback, this);
    w.exec();
  } else {
    confirm_close_callback(0);
  }
}

void DiaryWindow::confirm_close_callback(const int code) {
  if (code == 1)
    return;

  *(TheoreticalDiary::instance()->unsaved_changes) = false;
  TheoreticalDiary::instance()->diary_holder->key->clear();
  *(TheoreticalDiary::instance()->diary_holder->diary) = td::Diary();

  accept();
}

void DiaryWindow::action_save() {
  auto key = TheoreticalDiary::instance()->diary_holder->key;

  std::string primary_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/diary.dat";
  std::string backup_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/diary.dat.bak";
  std::string final;

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
      std::time(nullptr);
  nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);

  // If there is a password set, encrypt the diary
  if (key->size() == 32) {
    Encryptor::encrypt(*key, j.dump(), final);
  } else {
    final = j.dump();
  }

  auto success = Zipper::zip(primary_path, final);
  if (!success) {
    SaveError w(this);
    w.exec();
  } else {
    *(TheoreticalDiary::instance()->unsaved_changes) = false;
  }
}

void DiaryWindow::update_password() {
  UpdatePassword w(this);
  w.exec();
}
