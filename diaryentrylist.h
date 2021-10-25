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

#ifndef DIARYENTRYLIST_H
#define DIARYENTRYLIST_H

#include "diaryholder.h"

#include <QDate>
#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <optional>
#include <string>

namespace Ui {
class DiaryEntryList;
}

class DiaryEntryList : public QDialog {
  Q_OBJECT

public:
  explicit DiaryEntryList(QWidget *parent = nullptr);
  ~DiaryEntryList();
  void change_month(const QDate date);
  void render_month(std::optional<td::EntryMap *> entries);
  void rerender_current_month();
  QLabel *create_day_label(std::pair<const int, const td::Entry> const i);

public slots:
  void dropdown_changed(const int index);
  void date_changed(const QDate &date);
  void next_month();
  void last_month();
  void reset_month();

private:
  Ui::DiaryEntryList *ui;
  QDate *first_created;

  QString *s_base;
  QString *s_very_bad;
  QString *s_bad;
  QString *s_ok;
  QString *s_good;
  QString *s_very_good;
  QString *s_starred;
};

//
//
// Clickable label
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(const int &d, const std::string &t, QWidget *parent);
  ~ClickableLabel();

  static void get_trunc_first_line(std::string const &input, std::string &res);

signals:
  void clicked();

private:
  int *day;
  std::string *full_text;
  bool *expanded;

protected:
  void mouseReleaseEvent(QMouseEvent *event);
};

#endif // DIARYENTRYLIST_H
