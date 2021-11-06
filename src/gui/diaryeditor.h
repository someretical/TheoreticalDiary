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

#ifndef DIARYEDITOR_H
#define DIARYEDITOR_H

namespace td {
struct CalendarButtonData;
}

#include "../core/diaryholder.h"
#include "calendarbutton.h"

#include <QWidget>
#include <vector>

namespace Ui {
class DiaryEditor;
}

class DiaryEditor : public QWidget {
  Q_OBJECT

signals:
  void sig_re_render_buttons(const td::CalendarButtonData &data);

public:
  explicit DiaryEditor(QWidget *parent = nullptr);
  ~DiaryEditor();

  bool validate_entry();
  bool confirm_switch();

  bool current_date_changed;
  int current_month_offset;
  int last_selected_day;

  // This is an array of 6 stylesheets
  std::vector<QString> *rating_stylesheets;
  QString *base_stylesheet;
  QString *selected_stylesheet;
  QString *white_star;
  QString *black_star;

public slots:
  void apply_theme();

  // Calendar widget
  void render_month(const QDate &date,
                    const std::optional<td::YearMap::iterator> iter);
  void change_month(const QDate &date);
  void render_day(const td::CalendarButtonData &d, const bool &set_info_pane);
  void next_month();
  void prev_month();
  void month_changed(const int month);
  void year_changed(const QDate &date);
  void date_clicked(const int day);

  // Info pane
  void update_info_pane(const QDate &date, const td::Entry &entry);
  void update_day();
  void delete_day();
  void reset_day();

private:
  Ui::DiaryEditor *ui;
};

#endif // DIARYEDITOR_H