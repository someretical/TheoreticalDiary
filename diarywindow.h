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

#ifndef DIARYWINDOW_H
#define DIARYWINDOW_H

class TheoreticalCalendar;

#include "diaryentrylist.h"
#include "diaryholder.h"
#include "theoreticalcalendar.h"
#include "theoreticaldiary.h"

#include <QDate>
#include <QDialog>
#include <string>

namespace Ui {
class DiaryWindow;
}

class DiaryWindow : public QDialog {
  Q_OBJECT

public:
  explicit DiaryWindow(QWidget *parent = nullptr);
  ~DiaryWindow();
  static std::string get_day_suffix(const int day);
  void update_info_pane(const QDate &new_date);

  QDate *current_date;
  bool *current_date_changed;

public slots:
  void reject();
  void action_save();
  void update_password();
  void reset_date();
  void delete_entry();
  void update_entry();
  void export_diary();
  void changes_made();
  void tab_changed();

private:
  void _update_info_pane(const td::Entry &entry);

  Ui::DiaryWindow *ui;
  TheoreticalCalendar *calendar;
  DiaryEntryList *entry_list;
};

#endif // DIARYWINDOW_H
