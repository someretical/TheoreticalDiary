/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
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

#ifndef DIARYEDITOR_H
#define DIARYEDITOR_H

#include <QtWidgets>
#include <memory>
#include <optional>

#include "../core/internalmanager.h"
#include "../util/diarycalendarbutton.h"

namespace Ui {
class DiaryEditor;
}

class DiaryEditor : public QWidget {
    Q_OBJECT

signals:
    void sig_re_render_buttons(td::CalendarButtonData const &data);
    void sig_re_render(QDate const &date, bool const ignore_month_check);

public:
    explicit DiaryEditor(QDate const &date, QWidget *parent = nullptr);
    ~DiaryEditor();

    int current_month_offset;
    int last_selected_day;

    QShortcut *save_shortcut;

public slots:
    void update_theme();

    // Calendar widget.
    void render_month(QDate const &date, std::optional<td::YearMap::iterator> const &iter);
    void change_month(QDate const &date, bool const suppress_confirm);
    void render_day(td::CalendarButtonData const &d, bool const set_info_pane);
    void next_month();
    void prev_month();
    void month_changed(int const month);
    void year_changed(QDate const &date);
    void date_clicked(int const day);

    // Info pane.
    void update_info_pane(QDate const &date, td::Entry const &entry);
    void update_day(bool const suppress_error);
    void delete_day();
    void reset_day();

private:
    Ui::DiaryEditor *ui;

    DiaryCalendarButton *create_button(td::CalendarButtonData const &&d);
};

#endif // DIARYEDITOR_H
