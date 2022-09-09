/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THEORETICAL_DIARY_MONTHTABLEWIDGET_H
#define THEORETICAL_DIARY_MONTHTABLEWIDGET_H

class DiaryCalendar;

#include "gui/diary_menu/calendar/DiaryCalendar.h"

#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QDate>

class MonthTableWidget : public QTableWidget {
    Q_OBJECT

signals:
    void sigSelectionUpdated(const QDate &date);

public:
    explicit MonthTableWidget(const QDate &date, DiaryCalendar *calendar, QWidget *parent = nullptr);
    ~MonthTableWidget() override;

    void changeDate(const QDate &date);
    [[nodiscard]] auto getDate() const -> const QDate &;
    auto getCalendar() const -> DiaryCalendar *;

public slots:
    void updateSelectedDate(const QDate &date);

private:
    QDate m_date;
    DiaryCalendar *m_calendar;
};

class MonthTableStyleDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit MonthTableStyleDelegate(MonthTableWidget *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    MonthTableWidget *m_tableWidget;
};

#endif // THEORETICAL_DIARY_MONTHTABLEWIDGET_H
