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

#ifndef THEORETICALCALENDAR_H
#define THEORETICALCALENDAR_H

#include "diaryholder.h"
#include "diarywindow.h"
#include "theoreticaldiary.h"

#include <QDate>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <json.hpp>
#include <optional>
#include <vector>

namespace Ui {
class TheoreticalCalendar;
}

// Forward declaration of TheoreticalCalendar to fix cyclical reference
class TheoreticalCalendar;

namespace td {
struct CalendarButtonData {
  int day;
  std::optional<TheoreticalCalendar *> parent;
  std::optional<bool> important;
  std::optional<td::Rating> rating;
  std::optional<bool> selected;
};
} // namespace td

// CalendarButton class
class CalendarButton : public QPushButton {
  Q_OBJECT

signals:
  void sig_clicked(const int day);

public:
  explicit CalendarButton(const td::CalendarButtonData &d);
  ~CalendarButton();
  void update_aesthetic(const td::CalendarButtonData &d);

  td::CalendarButtonData *data;

public slots:
  void clicked_on();
};

// TheoreticalCalendar class
class TheoreticalCalendar : public QWidget {
  Q_OBJECT

signals:
  void sig_update_info(const QDate date);

public:
  explicit TheoreticalCalendar(QWidget *parent = nullptr);
  ~TheoreticalCalendar();
  void render_month(const QDate &first_day,
                    const std::optional<td::EntryMap *> &map);
  void rerender_day(const td::CalendarButtonData d);
  void change_month(const QDate first_day);

  QString *s_base;
  QString *s_default;
  QString *s_very_bad;
  QString *s_bad;
  QString *s_ok;
  QString *s_good;
  QString *s_very_good;
  QString *s_selected;
  QString *s_star_white;
  QString *s_star_black;

  int *current_month_offset;
  int *last_selected_day;

  QDate *first_created;

public slots:
  void clicked_on(const int day);
  void dropdown_changed(const int index);
  void date_changed(const QDate &date);
  void next_month();
  void last_month();

private:
  Ui::TheoreticalCalendar *ui;
};

#endif // THEORETICALCALENDAR_H
