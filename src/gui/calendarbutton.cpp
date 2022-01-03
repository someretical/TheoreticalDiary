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

#include "calendarbutton.h"
#include "diaryeditor.h"

CalendarButton::CalendarButton(td::CalendarButtonData const &d) : QPushButton(qobject_cast<QWidget *>(*d.parent))
{
    data = d;

    setAutoDefault(true);
    setCursor(QCursor(Qt::PointingHandCursor));
    setText(QString::number(*d.day));

    QFont f = font();
    f.setPointSize(16);
    setFont(f);

    // Apply new themes if requested
    connect(*d.parent, &DiaryEditor::sig_re_render_buttons, this, &CalendarButton::re_render, Qt::QueuedConnection);

    // Link click event through here to pass day variable through.
    connect(this, &CalendarButton::clicked, this, &CalendarButton::clicked_on, Qt::QueuedConnection);
    connect(this, &CalendarButton::sig_clicked, *d.parent, &DiaryEditor::date_clicked, Qt::QueuedConnection);

    re_render(d);
}

CalendarButton::~CalendarButton() {}

// Update the the following properties of the button:
// - Background colour
// - Icon
// - Whether it is selected or not
void CalendarButton::re_render(td::CalendarButtonData const &d)
{
    // Set stylesheet (determines colours).
    QString stylesheet((*data.parent)->base_stylesheet);

    // Set background star if necessary.
    // If the provided 'd' object does not have the 'important' property set, use the 'important' property from 'data'
    // instead. If the provided 'd' object DOES contain an 'important' property, update the 'important' property of
    // 'data'.
    data.important = std::optional(d.important.value_or(*data.important));
    data.rating = std::optional(d.rating.value_or(*data.rating));

    if (*data.important) {
        switch (*data.rating) {
        case td::Rating::Unknown:
            // Fall through
        case td::Rating::VeryBad:
            // Fall through
        case td::Rating::Bad:
            // Fall through
        case td::Rating::Ok:
            stylesheet.append((*data.parent)->white_star);
            break;
        case td::Rating::Good:
            // Fall through
        case td::Rating::VeryGood:
            stylesheet.append((*data.parent)->black_star);
            break;
        }
    }

    // Set colour scheme
    auto const r = d.rating.value_or(*data.rating);
    data.rating = std::optional(r);
    stylesheet.append(*((*data.parent)->rating_stylesheets)[static_cast<int>(r)]);

    // Give border if selected
    data.selected = std::optional(d.selected.value_or(*data.selected));
    if (*data.selected) {
        stylesheet.append((*data.parent)->selected_stylesheet);
    }

    setStyleSheet(stylesheet);
}

void CalendarButton::clicked_on()
{
    emit sig_clicked(*data.day);
}
