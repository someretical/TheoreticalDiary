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

#include "gui/diary_menu/calendar/MonthTableWidget.h"
#include "core/Config.h"
#include "core/Util.h"
#include "gui/Icons.h"
#include "gui/MainWindow.h"
#include "gui/styling/StyleManager.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QHeaderView>
#include <QPainter>
#include <QStyle>
#include <QtMath>

const int CELL_SIZE = 60;
const int BORDER_WIDTH = 4;
const int ROUNDNESS = 6;
const int DATE_ROLE = 1;

/**
 * Constructs a new QTableWidget for a single month in the calendar tab in the diary main menu
 *
 * @param date The first day of the month. QDate(year, month, 1). year and the 1 should be constant across all
 * MonthTableWidgets.
 * @param calendar
 * @param parent
 */
MonthTableWidget::MonthTableWidget(const QDate &date, DiaryCalendar *calendar, QWidget *parent)
    : QTableWidget(parent), m_calendar(calendar), m_date(date)
{
    setItemDelegate(new MonthTableStyleDelegate(this));

    setColumnCount(7);
    setShowGrid(false);
    setCornerButtonEnabled(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    changeDate(date);

    horizontalHeader()->setMinimumSectionSize(CELL_SIZE);
    horizontalHeader()->setDefaultSectionSize(CELL_SIZE);
    horizontalHeader()->setVisible(false);

    verticalHeader()->setMinimumSectionSize(CELL_SIZE);
    verticalHeader()->setDefaultSectionSize(CELL_SIZE);
    verticalHeader()->setVisible(false);

    connect(this, &MonthTableWidget::itemDoubleClicked, this,
        [this](QTableWidgetItem *i) { qDebug() << i->data(DATE_ROLE); });
    connect(this, &MonthTableWidget::itemSelectionChanged, [this]() {
        const auto &items = selectedItems();

        if (!items.empty()) {
            emit sigSelectionUpdated(items[0]->data(DATE_ROLE).toDate());
        }
    });
}

MonthTableWidget::~MonthTableWidget() = default;

/**
 * Changes the month displayed by the QTableWidget
 *
 * @param date Only the year and the month matter.
 */
void MonthTableWidget::changeDate(const QDate &date)
{
    int rows = qCeil(date.daysInMonth() / 7.0f);
    int total = rows * 7;
    setRowCount(rows);
    setFixedSize(CELL_SIZE * 7, CELL_SIZE * rows);

    for (int i = 0; i < total; ++i) {
        const int x = i % 7;
        const int y = i / 7;

        auto itemWidget = item(y, x);
        if (!itemWidget) {
            itemWidget = new QTableWidgetItem();
            setItem(y, x, itemWidget);
        }

        const auto specificDate = QDate(date.year(), date.month(), i + 1);

        itemWidget->setData(DATE_ROLE, specificDate);

        if (i >= date.daysInMonth()) {
            itemWidget->setToolTip("");
            itemWidget->setFlags(Qt::NoItemFlags);
        }
        else {
            itemWidget->setToolTip(specificDate.toString("dddd d'%1'").arg(getOrdinalSuffix(specificDate.day())));
            itemWidget->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
    }

    m_date = date;
}

auto MonthTableWidget::getDate() const -> const QDate &
{
    return m_date;
}

/**
 * Removes the selection outline from the previous selected date if the user clicks on a new date in another
 * QTableWidget
 * @param date The exact date. The year, month and day all matter.
 */
void MonthTableWidget::updateSelectedDate(const QDate &date)
{
    if (date.month() != m_date.month()) {
        for (auto i : selectedItems()) {
            setItemSelected(i, i->data(DATE_ROLE).toDate().day() == date.day());
        }
    }
}

auto MonthTableWidget::getCalendar() const -> DiaryCalendar *
{
    return m_calendar;
}

/* ================================================================================================================== */

MonthTableStyleDelegate::MonthTableStyleDelegate(MonthTableWidget *parent)
    : QStyledItemDelegate(parent), m_tableWidget(parent)
{
}

void MonthTableStyleDelegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // This condition has to be outside for some reason. The background colour is not properly set if the function is
    // nested inside a ternary operator inside the .color() function.
    if (mainWindow()->isActiveWindow()) {
        painter->fillRect(opt.rect, QApplication::style()->standardPalette().color(QPalette::Active, QPalette::Window));
    }
    else {
        painter->fillRect(
            opt.rect, QApplication::style()->standardPalette().color(QPalette::Inactive, QPalette::Window));
    }

    if ((index.row() * 7) + index.column() + 1 <= m_tableWidget->getDate().daysInMonth()) {
        const auto adjustedSize = CELL_SIZE - BORDER_WIDTH - (BORDER_WIDTH / 2);
        const auto topLeft = opt.rect.topLeft();
        const auto rect = QRect(topLeft.x() + BORDER_WIDTH, topLeft.y() + BORDER_WIDTH, adjustedSize, adjustedSize);
        const auto &data = m_tableWidget->getCalendar()->getEntryPreviewData();
        const auto &date = index.data(DATE_ROLE).toDate();
        const auto iter = data.find(date);
        const auto rating = iter == data.end() ? DiaryRating::Unknown : iter->rating;
        const auto &overlayColour = styleManager().getTextColourFromTheme(rating);
        const auto &colour = styleManager().getRatingColour(rating);

        QPainterPath cell;
        cell.addRoundedRect(rect, ROUNDNESS, ROUNDNESS);
        painter->fillPath(cell, QBrush(colour));

        if (opt.state & QStyle::State_Selected) {
            if (config()->get(Config::GUI_Theme) == TD::Theme::Dark) {
                painter->strokePath(cell, QPen(QBrush(colour.lighter(110)), BORDER_WIDTH));
            }
            else {
                painter->strokePath(cell, QPen(QBrush(colour.darker(130)), BORDER_WIDTH));
            }
        }

        auto font = QApplication::font();
        if (date == m_tableWidget->getCalendar()->getIRLDate()) {
            font.setItalic(true);
            font.setBold(true);
        }
        painter->setFont(font);
        painter->setPen(styleManager().getTextColourFromTheme(rating));
        painter->drawText(rect.translated(-4, 0), Qt::AlignTop | Qt::AlignRight, QString::number(date.day()));

        if (iter->important) {
            const auto pixmap = icons().icon("star", true, overlayColour).pixmap(40, 40);
            painter->setOpacity(0.6);
            painter->drawPixmap(topLeft.x() + 10 + (BORDER_WIDTH / 2), topLeft.y() + 10 + (BORDER_WIDTH / 2), pixmap);
        }
    }

    painter->restore();
}
