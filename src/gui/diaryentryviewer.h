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

#ifndef DIARYENTRYVIEWER_H
#define DIARYENTRYVIEWER_H

#include "diaryeditor.h"

#include <QtWidgets>

namespace Ui {
class DiaryEntryViewer;
}

class DiaryEntryViewer : public QWidget {
  Q_OBJECT

signals:
  void sig_re_render_theme();

public:
  explicit DiaryEntryViewer(const DiaryEditor *editor,
                            QWidget *parent = nullptr);
  ~DiaryEntryViewer();

  QDate *current_month;
  std::vector<QString> *rating_stylesheets;
  QString *white_star;
  QString *black_star;

public slots:
  void apply_theme();
  void change_month(const QDate &date, const bool ignore_month_check);
  void next_month();
  void prev_month();
  void month_changed(const int month);
  void year_changed(const QDate &date);

private:
  Ui::DiaryEntryViewer *ui;
};

namespace td {
struct LabelData {
  DiaryEntryViewer *parent;
  int day;
  td::Rating rating;
  bool special;
};
} // namespace td

class DiaryEntryDayLabel : public QLabel {
  Q_OBJECT

public:
  explicit DiaryEntryDayLabel(const td::LabelData &d,
                              QWidget *parent = nullptr);
  ~DiaryEntryDayLabel();

  td::LabelData data;

public slots:
  void apply_theme();
};

class DiaryEntryDayMessage : public QLabel {
  Q_OBJECT

public:
  explicit DiaryEntryDayMessage(const std::string &m,
                                QWidget *parent = nullptr);
  ~DiaryEntryDayMessage();

  static void get_trunc_first_line(const std::string &input, std::string &res);

  std::string *message;
  bool expanded;

public slots:
  void apply_theme();

protected:
  void mouseDoubleClickEvent(const QMouseEvent *event);
};

#endif // DIARYENTRYVIEWER_H
