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

#include "diaryentryviewer.h"
#include "ui_diaryentryviewer.h"

int const MAX_LINE_LEN = 110;
int const DAY_LABEL_SIZE = 50;

DiaryEntryViewer::DiaryEntryViewer(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryEntryViewer)
{
    ui->setupUi(this);

    rating_stylesheets = std::vector<std::unique_ptr<QString>>();
    black_star = QString();
    white_star = QString();

    // Navigator slots
    connect(ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &DiaryEntryViewer::month_changed, Qt::QueuedConnection);
    connect(ui->year_edit, &QDateEdit::dateChanged, this, &DiaryEntryViewer::year_changed, Qt::QueuedConnection);
    connect(ui->next_month, &QPushButton::clicked, this, &DiaryEntryViewer::next_month, Qt::QueuedConnection);
    connect(ui->prev_month, &QPushButton::clicked, this, &DiaryEntryViewer::prev_month, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryEntryViewer::change_month,
        Qt::QueuedConnection);
    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEntryViewer::update_theme,
        Qt::QueuedConnection);
    update_theme();

    // Make scroll bar hit the bottom.
    QTimer::singleShot(0, this, [&]() {
        ui->scrollArea->widget()->adjustSize();
        ui->scrollArea->widget()->update();
        ui->scrollArea->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
    });

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryEntryViewer::~DiaryEntryViewer()
{
    delete ui;
}

void DiaryEntryViewer::update_theme()
{
    auto const &theme = InternalManager::instance()->get_theme_str();

    QFile file(QString(":/%1/diary_entry_list/base.qss").arg(theme));
    file.open(QIODevice::ReadOnly);
    ui->scrollArea->setStyleSheet(file.readAll());
    file.close();

    file.setFileName(QString(":/%1/diaryentryviewer.qss").arg(theme));
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();

    file.setFileName(":/global/white_star.qss");
    file.open(QIODevice::ReadOnly);
    white_star = file.readAll();
    file.close();

    file.setFileName(":/global/black_star.qss");
    file.open(QIODevice::ReadOnly);
    black_star = file.readAll();
    file.close();

    for (auto &ss_ptr : rating_stylesheets)
        ss_ptr.reset();

    rating_stylesheets.clear();

    for (int i = 0; i < 6; ++i) {
        file.setFileName(QString(":/%1/diary_entry_list/%2.qss").arg(theme, QString::number(i)));
        file.open(QIODevice::ReadOnly);
        rating_stylesheets.push_back(std::make_unique<QString>(file.readAll()));
        file.close();
    }

    emit sig_update_labels();
}

void DiaryEntryViewer::change_month(QDate const &date)
{
    // Remove everything from current grid.
    QLayoutItem *child;
    while ((child = ui->entry_grid->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    // Update the selector UI.
    ui->month_dropdown->blockSignals(true);
    ui->year_edit->blockSignals(true);

    if (date.isValid()) {
        ui->month_dropdown->setCurrentIndex(date.month() - 1);
        ui->year_edit->setDate(date);
    }
    else {
        ui->month_dropdown->setCurrentIndex(current_date.month() - 1);
        ui->year_edit->setDate(current_date);
    }

    ui->month_dropdown->blockSignals(false);
    ui->year_edit->blockSignals(false);

    auto const &opt = DiaryHolder::instance()->get_monthmap(date.isValid() ? date : current_date);
    if (!opt) {
        auto label = new QLabel("It seems there are no entries yet for this month...", this);
        label->setAlignment(Qt::AlignCenter);
        auto f = label->font();
        f.setPointSize(11);
        label->setFont(f);

        current_date = date;
        return ui->entry_grid->addWidget(label);
    }

    int row_counter = 0;
    for (auto const &i : (*opt)->second) {
        auto const &[important, rating, message, dummy] = i.second;

        // Don't add any days that don't have text entries.
        if (message.empty())
            continue;

        // Ideally, the message should have been trimmed when the entry itself was saved. However, that was not always
        // the case so the message has to be trimmed here for backwards compatability.
        auto msg_copy = message;
        auto day = new DiaryEntryDayLabel(td::LabelData{this, i.first, rating, important}, this);
        auto formatted_msg = new DiaryEntryDayMessage(misc::trim(msg_copy), this);
        auto spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        ui->entry_grid->addWidget(day, row_counter, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
        ui->entry_grid->addWidget(formatted_msg, row_counter, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
        ui->entry_grid->addItem(spacer, row_counter++, 2);
    }

    if (0 == row_counter) {
        auto label = new QLabel("It seems there are no entries yet for this month...", this);
        label->setAlignment(Qt::AlignCenter);
        auto f = label->font();
        f.setPointSize(11);
        label->setFont(f);

        ui->entry_grid->addWidget(label);
    }

    current_date = date;

    qDebug() << "Changed diary entry viewer month:" << date;
}

void DiaryEntryViewer::next_month()
{
    QDate const next = ui->year_edit->date().addMonths(1);
    if (next.isValid()) {
        change_month(next);

        // Make scroll bar hit top.
        QTimer::singleShot(0, this, [&]() {
            ui->scrollArea->widget()->adjustSize();
            ui->scrollArea->widget()->update();
            ui->scrollArea->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        });
    }
}

void DiaryEntryViewer::prev_month()
{
    QDate const prev = ui->year_edit->date().addMonths(-1);
    if (prev.isValid()) {
        change_month(prev);

        // Make scroll bar hit bottom.
        QTimer::singleShot(0, this, [&]() {
            ui->scrollArea->widget()->adjustSize();
            ui->scrollArea->widget()->update();
            ui->scrollArea->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
        });
    }
}

void DiaryEntryViewer::month_changed(int)
{
    change_month(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryEntryViewer::year_changed(QDate const &date)
{
    if (date.isValid())
        change_month(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

/*
 * DiaryEntryDayLabel class
 */
DiaryEntryDayLabel::DiaryEntryDayLabel(td::LabelData const &d, QWidget *parent) : QLabel(parent)
{
    data = d;
    setText(QString::number(d.day));
    setFixedHeight(DAY_LABEL_SIZE);
    setFixedWidth(DAY_LABEL_SIZE);
    setAlignment(Qt::AlignCenter);

    QFont f = font();
    f.setPointSize(14);
    f.setBold(true);
    setFont(f);

    update();

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEntryDayLabel::update_theme,
        Qt::QueuedConnection);
    connect(
        d.parent, &DiaryEntryViewer::sig_update_labels, this, &DiaryEntryDayLabel::update_theme, Qt::QueuedConnection);
    update_theme();
}

DiaryEntryDayLabel::~DiaryEntryDayLabel() {}

void DiaryEntryDayLabel::update_theme()
{
    // Set colour theme.
    QString stylesheet(*(data.parent->rating_stylesheets)[static_cast<int>(data.rating)]);

    // Set background star if necessary.
    if (data.special) {
        switch (data.rating) {
        case td::Rating::Unknown:
            // Fall through
        case td::Rating::VeryBad:
            // Fall through
        case td::Rating::Bad:
            // Fall through
        case td::Rating::Ok:
            stylesheet.append(data.parent->white_star);
            break;
        case td::Rating::Good:
            // Fall through
        case td::Rating::VeryGood:
            stylesheet.append(data.parent->black_star);
            break;
        }
    }

    setStyleSheet(stylesheet);
}

/*
 * DiaryEntryDayMessage class
 */
DiaryEntryDayMessage::DiaryEntryDayMessage(std::string const &m, QWidget *parent) : QLabel(parent)
{
    message = std::string(m);
    expanded = false;

    // Set text.
    setText(misc::get_trunc_first_line(m, MAX_LINE_LEN).data());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    QFont f = font();
    f.setPointSize(14);
    setFont(f);

    update();

    setCursor(QCursor(Qt::PointingHandCursor));

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEntryDayMessage::update_theme,
        Qt::QueuedConnection);
    update_theme();
}

DiaryEntryDayMessage::~DiaryEntryDayMessage() {}

void DiaryEntryDayMessage::update_theme() {}

void DiaryEntryDayMessage::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (expanded)
        setText(misc::get_trunc_first_line(message, MAX_LINE_LEN).data());
    else
        setText(message.data());

    update();

    expanded = !expanded;
}
