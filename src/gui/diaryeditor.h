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

public:
    explicit DiaryEditor(QDate const &date, QWidget *parent = nullptr);
    ~DiaryEditor();

    QDate m_current_date;
    int m_current_month_offset;
    QShortcut *m_save_bind;
    QShortcut *m_next_day_bind;
    QShortcut *m_prev_day_bind;
    QShortcut *m_next_month_bind;
    QShortcut *m_prev_month_bind;
    // Saves the state of the entry when it was first loaded. Used to decide if the save prompt needs showing.
    td::Entry m_last_entry_snapshot;

public slots:
    // Calendar widget.
    void setup_buttons();
    void render_month(std::optional<td::YearMap::iterator> const &iter);
    void change_month(QDate const &date);
    void render_day(td::CalendarButtonData const &d, bool const set_info_pane);
    void next_month();
    void prev_month();
    void month_changed(int const month);
    void year_changed(QDate const &date);
    void date_clicked(int const day);
    void next_day();
    void prev_day();

    // Reference info.
    void emotions_list_button_clicked();
    void rain_guide_button_clicked();
    void gratitude_guide_button_clicked();

    // Info pane.
    void update_info_pane(QDate const &date, td::Entry const &entry);
    bool update_day(bool const suppress_error = false);
    void delete_day();
    void reset_day();

private:
    Ui::DiaryEditor *m_ui;

    bool compare_snapshots();
};

#endif // DIARYEDITOR_H
