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

#ifndef THEORETICAL_DIARY_DIARYCALENDAR_H
#define THEORETICAL_DIARY_DIARYCALENDAR_H

class MonthTableWidget;

#include "gui/diary_menu/calendar/MonthTableWidget.h"
#include "gui/diary_menu/DiaryWidget.h"

#include <QLayout>
#include <QWidget>
#include <QHash>

QT_BEGIN_NAMESPACE
namespace Ui {
class DiaryCalendar;
}
QT_END_NAMESPACE

class DiaryCalendar : public QWidget {
    Q_OBJECT

signals:
    void sigUpdateSelectedDate(const QDate &date);

public:
    enum UpdateType {
        SingleDay,
        WholeYear
    };

    explicit DiaryCalendar(QWidget *parent = nullptr);
    ~DiaryCalendar() override;
    void updateEntryPreviewData(const QDate &date, const UpdateType type);
    const QHash<QDate, EntryPreviewData> &getEntryPreviewData() const;
    const QDate &getIRLDate() const;

    auto getDiaryWidget() -> DiaryWidget *;
    DiaryWidget *m_diaryWidget;

public slots:
    void changeYear(const QDate &date);

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::DiaryCalendar *m_ui;
    QVector<MonthTableWidget *> m_calendarTables;
    QVector<QLayout *> m_calendarLayouts;
    QDate m_irlDate;
    QHash<QDate, EntryPreviewData> m_entryPreviewData;

    void initMonthTables();
};

#endif // THEORETICAL_DIARY_DIARYCALENDAR_H
