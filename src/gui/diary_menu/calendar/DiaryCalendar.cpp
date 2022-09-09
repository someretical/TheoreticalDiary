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

#include "gui/diary_menu/calendar/DiaryCalendar.h"
#include "core/Diary.h"
#include "gui/Icons.h"
#include "gui/MainWindow.h"
#include "gui/diary_menu/DiaryMainMenuWidget.h"
#include "ui_DiaryCalendar.h"

#include <Logger.h>
#include <QBoxLayout>
#include <QDate>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>

DiaryCalendar::DiaryCalendar(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::DiaryCalendar), m_irlDate(QDate::currentDate())
{
    m_ui->setupUi(this);
    m_ui->nextYearButton->setIcon(icons().icon("calendar-next-year"));
    m_ui->previousYearButton->setIcon(icons().icon("calendar-previous-year"));
    m_ui->yearEdit->setDate(m_irlDate);

    initMonthTables();

    connect(m_ui->previousYearButton, &QToolButton::clicked, [this]() {
        changeYear(m_ui->yearEdit->date().addYears(-1));
        m_ui->calendarScrollArea->verticalScrollBar()->setValue(
            m_ui->calendarScrollArea->verticalScrollBar()->maximum());
    });
    connect(m_ui->nextYearButton, &QToolButton::clicked, [this]() {
        changeYear(m_ui->yearEdit->date().addYears(1));
        m_ui->calendarScrollArea->verticalScrollBar()->setValue(0);
    });
    connect(m_ui->yearEdit, SIGNAL(dateChanged(const QDate &)), this, SLOT(changeYear(const QDate &)));
}

DiaryCalendar::~DiaryCalendar()
{
    delete m_ui;
}

/**
 * This function runs the first time the calendar is rendered and it scrolls to the month containing m_irlDate. It also
 * selects the current day.
 *
 * @param event
 */
void DiaryCalendar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;

        int offset = 0;
        auto month = QDate(m_irlDate.year(), 1, 1);
        while (month.month() < m_irlDate.month()) {
            offset += m_calendarLayouts[month.month() - 1]->geometry().height() + 30;
            month = month.addMonths(1);
        }

        m_ui->calendarScrollArea->verticalScrollBar()->setValue(offset);

        const int x = (m_irlDate.day() - 1) % 7;
        const int y = (m_irlDate.day() - 1) / 7;
        m_calendarTables[m_irlDate.month() - 1]->item(y, x)->setSelected(true);
    }
}

/**
 * Creates the QTableWidgets for each month
 */
void DiaryCalendar::initMonthTables()
{
    auto month = QDate(m_irlDate.year(), 1, 1);
    while (month.year() == m_irlDate.year()) {
        auto columnLayout = new QVBoxLayout();
        columnLayout->setContentsMargins(0, 0, 0, 0);
        columnLayout->setSpacing(6);

        auto name = new QLabel();
        QFont font = name->font();
        font.setBold(true);
        font.setPointSize(font.pointSize() + 2);
        font.setBold(true);
        name->setFont(font);
        name->setText(month.toString("MMMM"));
        name->setAlignment(Qt::AlignHCenter);
        columnLayout->addWidget(name);

        auto rowLayout = new QHBoxLayout();
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(0);
        rowLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

        auto calendar = new MonthTableWidget(month, this, nullptr);
        connect(
            calendar, SIGNAL(sigSelectionUpdated(const QDate &)), this, SIGNAL(sigUpdateSelectedDate(const QDate &)));
        connect(this, SIGNAL(sigUpdateSelectedDate(const QDate &)), calendar, SLOT(updateSelectedDate(const QDate &)));
        m_calendarTables.push_back(calendar);
        rowLayout->addWidget(calendar);

        rowLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
        columnLayout->addLayout(rowLayout);

        m_calendarLayouts.push_back(columnLayout);

        m_ui->calendarLayout->addLayout(columnLayout);

        month = month.addMonths(1);
    }
}

/**
 * Changes the year being displayed by all the QTableWidgets
 *
 * @param date The date of the new year. Only the year matters.
 */
void DiaryCalendar::changeYear(const QDate &date)
{
    mainWindow()->setEnabled(false);

    m_ui->previousYearButton->setEnabled(date.addYears(-1).isValid());
    m_ui->nextYearButton->setEnabled(date.addYears(1).isValid());
    m_ui->yearEdit->setDate(date);

    for (int i = 0; i < m_calendarTables.size(); ++i) {
        m_calendarTables[i]->changeDate(QDate(date.year(), i + 1, 1));
    }

    mainWindow()->setEnabled(true);

    LOG_INFO() << "Changed year in diary calendar to" << date;
}

auto DiaryCalendar::getDiaryWidget() -> DiaryWidget *
{
    return qobject_cast<DiaryMainMenuWidget *>(parentWidget()->parentWidget()->parentWidget())->getDiaryWidget();
}

auto DiaryCalendar::getEntryPreviewData() const -> const QHash<QDate, EntryPreviewData> &
{
    return m_entryPreviewData;
}

/**
 * Updates the entry preview data
 *
 * @param date If the type is SingleDay, then the year, month and day matter. If the type is WholeYear, then only the
 * year matters.
 * @param type SingleDay updates only a single day. WholeYear updates the entire year.
 */
void DiaryCalendar::updateEntryPreviewData(const QDate &date, const UpdateType type)
{
    auto &diary = getDiaryWidget()->getDiary();

    const auto yearIter = diary.years.find(date.year());
    if (yearIter == diary.years.end())
        return;

    switch (type) {
    case SingleDay: {
        const auto entryIter = yearIter->entries.find(date);
        if (entryIter == yearIter->entries.end())
            return;

        m_entryPreviewData[date] = EntryPreviewData{entryIter->important, entryIter->rating};
        break;
    }
    case WholeYear: {
        QDate month(date.year(), 1, 1);

        while (month.year() == date.year()) {
            QDate day(date.year(), month.month(), 1);

            while (day.month() == month.month()) {
                const auto entryIter = yearIter->entries.find(day);
                if (entryIter != yearIter->entries.end()) {
                    m_entryPreviewData[day] = EntryPreviewData{entryIter->important, entryIter->rating};
                }

                day = day.addDays(1);
            }

            month = month.addMonths(1);
        }
        break;
    }
    }
}

auto DiaryCalendar::getIRLDate() const -> const QDate &
{
    return m_irlDate;
}
