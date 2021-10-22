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
#include "confirmdelete.h"
#include "diaryholder.h"
#include "encryptor.h"
#include "missingpermissions.h"
#include "saveerror.h"
#include "theoreticalcalendar.h"
#include "theoreticaldiary.h"
#include "ui_diarywindow.h"
#include "unsavedchanges.h"
#include "updatepassword.h"
#include "zipper.h"

#include <QAction>
#include <QDate>
#include <QDateTime>
#include <QFileDialog>
#include <QString>
#include <ctime>
#include <fstream>
#include <json.hpp>
#include <optional>
#include <string>

DiaryWindow::DiaryWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::DiaryWindow) {
  ui->setupUi(this);

  QFile s(":/styles/diarywindow.qss");
  s.open(QIODevice::ReadOnly);
  QString s_ = s.readAll();
  setStyleSheet(s_);

  QFile s1(":/styles/diaryeditor.qss");
  s1.open(QIODevice::ReadOnly);
  QString s1_ = s1.readAll();
  // For some reason, calling this->setStyleSheet() has no effect on the
  // contents of the tab widget. So we have to call it on the tab widget
  // specifically.
  ui->editor->setStyleSheet(s1_);

  QDate d = QDate::currentDate();
  current_date = new QDate(d.year(), d.month(), d.day());
  current_date_changed = new bool(false);

  calendar = new TheoreticalCalendar(this);
  ui->calender_box->addWidget(calendar, Qt::AlignHCenter | Qt::AlignTop);

  ui->status_text->setText("");
  // Don't need to clear last_edited text here since initializing
  // TheoreticalCalendar will already set it.

  auto action = findChild<QAction *>("action_save");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::action_save);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::reject);

  action = findChild<QAction *>("action_update_password");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::update_password);

  action = findChild<QAction *>("action_reset");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::reset_date);

  action = findChild<QAction *>("action_update_entry");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::update_entry);

  action = findChild<QAction *>("action_delete_entry");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::delete_entry);

  action = findChild<QAction *>("action_export");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::export_diary);

  action = findChild<QAction *>("action_changes_made");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::changes_made);
}

DiaryWindow::~DiaryWindow() {
  delete ui;
  delete calendar;
  delete current_date;
  delete current_date_changed;
}

void DiaryWindow::changes_made() { *current_date_changed = true; }

// Refocus calendar on current date.
void DiaryWindow::reset_date() {
  calendar->change_month(*calendar->first_created);
}

// Create dialog confirming deleting the currently selected entry.
void DiaryWindow::delete_entry() {
  ConfirmDelete w(this);

  if (w.exec() != QDialog::Accepted)
    return;

  auto year_map = &TheoreticalDiary::instance()->diary_holder->diary->years;
  auto year_iter = year_map->find(current_date->year());

  if (year_iter != year_map->end()) {
    auto month_map = &year_iter->second.months;
    auto month_iter = month_map->find(current_date->month());

    if (month_iter != month_map->end()) {
      month_iter->second.days.erase(current_date->day());
    }
  }

  td::CalendarButtonData d{current_date->day(), std::nullopt,
                           std::make_optional<bool>(false),
                           std::make_optional<td::Rating>(td::Rating::Unknown),
                           std::make_optional<bool>(true)};
  calendar->rerender_day(d);

  td::Entry e{0, false, td::Rating::Unknown, "", 0};
  _update_info_pane(e);

  TheoreticalDiary::instance()->changes_made();
  *current_date_changed = false;
}

void DiaryWindow::update_entry() {
  if (td::Rating::Unknown ==
      static_cast<td::Rating>(ui->rating_dropdown->currentIndex())) {
    return ui->status_text->setText("Missing rating");
  }

  // Find/create path to new entry
  // https://stackoverflow.com/a/101980

  // Find/create YearContainer within Diary.years
  auto year_map = &TheoreticalDiary::instance()->diary_holder->diary->years;
  td::YearMap::iterator year_iter = year_map->lower_bound(current_date->year());

  if (year_iter == year_map->end() ||
      year_map->key_comp()(current_date->year(), year_iter->first)) {
    td::YearContainer year_container{current_date->year(), td::MonthMap()};

    year_iter = year_map->insert(
        year_iter,
        td::YearMap::value_type(current_date->year(), year_container));
  } // else { /* K, V already exist */ }

  // Find/create MonthContainer within the YearContainer.months
  auto month_map = &(year_iter->second.months);
  td::MonthMap::iterator month_iter =
      month_map->lower_bound(current_date->month());

  if (month_iter == month_map->end() ||
      month_map->key_comp()(current_date->month(), month_iter->first)) {
    td::MonthContainer month_container{current_date->month(), td::EntryMap()};

    month_iter = month_map->insert(
        month_iter,
        td::MonthMap::value_type(current_date->month(), month_container));
  } // else { /* K, V already exist */ }

  td::Entry new_entry = {
      current_date->day(), ui->is_important->isChecked(),
      static_cast<td::Rating>(ui->rating_dropdown->currentIndex()),
      ui->text_entry->toPlainText().toStdString(), std::time(nullptr)};

  // Find/create Entry within the MonthContainer.days
  auto entry_map = &(month_iter->second.days);
  td::EntryMap::iterator entry_iter =
      entry_map->lower_bound(current_date->day());

  if (entry_iter == entry_map->end() ||
      entry_map->key_comp()(current_date->day(), entry_iter->first)) {
    entry_iter = entry_map->insert(
        entry_iter, td::EntryMap::value_type(current_date->day(), new_entry));
  } else {
    entry_iter->second = new_entry;
  }

  td::CalendarButtonData data{
      current_date->day(), std::nullopt,
      std::make_optional<bool>(ui->is_important->isChecked()),
      std::make_optional<td::Rating>(
          static_cast<td::Rating>(ui->rating_dropdown->currentIndex())),
      std::nullopt};
  calendar->rerender_day(data);

  TheoreticalDiary::instance()->changes_made();
  *current_date_changed = false;
}

void DiaryWindow::export_diary() {
  auto filename =
      QFileDialog::getSaveFileName(this, "Export diary", QDir::homePath());

  if (filename.size() == 0)
    return;

  std::ofstream dst(filename.toStdString());

  if (!dst.fail()) {
    nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);
    dst << j.dump(4);
    dst.close();
    return;
  }

  MissingPermissions w(this);
  w.exec();
}

// Internal method for updating the non heading part of the info pane.
void DiaryWindow::_update_info_pane(const td::Entry &entry) {
  ui->rating_dropdown->blockSignals(true);
  ui->is_important->blockSignals(true);
  ui->text_entry->blockSignals(true);

  if (0 == entry.last_updated) {
    ui->last_edited->setText("");
  } else {
    QDateTime last_edited;
    last_edited.setTime_t(entry.last_updated);
    // Thanks stackoverflow ;)
    ui->last_edited->setText("Last edited " +
                             last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
  }

  ui->rating_dropdown->setCurrentIndex(static_cast<int>(entry.rating));
  ui->is_important->setChecked(entry.important);
  ui->text_entry->setPlainText(QString::fromStdString(entry.message));

  ui->rating_dropdown->blockSignals(false);
  ui->is_important->blockSignals(false);
  ui->text_entry->blockSignals(false);
}

// Updates the heading of the info pane and the rest of the info pane if the
// entry exists.
void DiaryWindow::update_info_pane(const QDate &new_date) {
  ui->status_text->setText("");

  // Don't bother updating if the user spam clicks the same day.
  if (current_date->year() == new_date.year() &&
      current_date->month() == new_date.month() &&
      current_date->day() == new_date.day())
    return;

  *current_date = new_date;

  ui->date_placeholder->setText(
      QString::number(new_date.day()) +
      QString::fromStdString(DiaryWindow::get_day_suffix(new_date.day())) +
      new_date.toString(" MMMM yyyy"));

  // Get iterators
  auto year_map = TheoreticalDiary::instance()->diary_holder->diary->years;
  auto year_iter = year_map.find(new_date.year());

  if (year_iter != year_map.end()) {
    auto month_map = year_iter->second.months;
    auto month_iter = month_map.find(new_date.month());

    if (month_iter != month_map.end()) {
      auto entry_map = month_iter->second.days;
      auto entry_iter = entry_map.find(new_date.day());

      if (entry_iter != entry_map.end())
        return _update_info_pane(entry_iter->second);
    }
  }

  _update_info_pane(td::Entry{-1, false, td::Rating::Unknown, "", 0});
}

// Called when the user presses the X button or the quit button.
void DiaryWindow::reject() {
  if (*TheoreticalDiary::instance()->unsaved_changes || *current_date_changed) {
    UnsavedChanges w(this);

    if (w.exec() != QDialog::Accepted)
      return;
  }

  // Reset values just to be safe.
  *TheoreticalDiary::instance()->unsaved_changes = false;
  TheoreticalDiary::instance()->diary_holder->key->clear();
  *TheoreticalDiary::instance()->diary_holder->diary = td::Diary();

  // accept() does NOT trigger reject so there is no infinite loop here.
  accept();
}

// Save the current entry to disk.
void DiaryWindow::action_save() {
  std::string primary_path =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat";
  std::string backup_path =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat.bak";

  // Backup existing diary first.
  std::ifstream src(primary_path, std::ios::binary);
  if (!src.fail()) {
    std::ofstream dst(backup_path, std::ios::binary);
    dst << src.rdbuf();
    dst.close();
  }
  src.close();

  // Update last_updated.
  TheoreticalDiary::instance()->diary_holder->diary->metadata.last_updated =
      std::time(nullptr);
  nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);

  // Gzip JSON
  std::string compressed;
  std::string decompressed = j.dump();
  Zipper::zip(compressed, decompressed);

  // Encrypt
  std::string encrypted;
  Encryptor::encrypt(*TheoreticalDiary::instance()->diary_holder->key,
                     compressed, encrypted);

  // Write to file
  std::ofstream diary_file(primary_path, std::ios::binary);
  if (!diary_file.fail()) {
    diary_file << encrypted;
    diary_file.close();
    *TheoreticalDiary::instance()->unsaved_changes = false;
    *current_date_changed = false;

  } else {
    SaveError w(this);
    w.exec();
  }
}

void DiaryWindow::update_password() {
  UpdatePassword w(this);
  w.exec();
}

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
