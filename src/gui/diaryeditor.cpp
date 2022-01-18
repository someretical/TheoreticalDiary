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

#include "diaryeditor.h"
#include "../core/diaryholder.h"
#include "../util/custommessageboxes.h"
#include "../util/misc.h"
#include "ui_diaryeditor.h"

DiaryEditor::DiaryEditor(QDate const &date, QWidget *parent) : QWidget(parent), ui(new Ui::DiaryEditor)
{
    ui->setupUi(this);
    ui->alert_text->setText("");
    ui->alert_text->update();

    current_month_offset = 0;
    last_selected_day = 0;
    last_entry_snapshot = td::Entry{false, td::Rating::Unknown, "", 0};

    // Ctrl S to save the diary.
    save_shortcut = new QShortcut(QKeySequence::Save, this);
    save_shortcut->setAutoRepeat(false);
    connect(
        save_shortcut, &QShortcut::activated, this, [&]() { update_day(false); }, Qt::QueuedConnection);

    // Calendar widget actions
    // See https://doc.qt.io/qt-5/qcombobox.html#currentIndexChanged for why QOverload<int> is needed.
    connect(ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DiaryEditor::month_changed,
        Qt::QueuedConnection);
    connect(ui->year_edit, &QDateEdit::dateChanged, this, &DiaryEditor::year_changed, Qt::QueuedConnection);
    connect(ui->next_month, &QPushButton::clicked, this, &DiaryEditor::next_month, Qt::QueuedConnection);
    connect(ui->prev_month, &QPushButton::clicked, this, &DiaryEditor::prev_month, Qt::QueuedConnection);

    // Info pane actions.
    connect(
        ui->update_button, &QPushButton::clicked, this, [&]() { update_day(false); }, Qt::QueuedConnection);
    connect(ui->delete_button, &QPushButton::clicked, this, &DiaryEditor::delete_day, Qt::QueuedConnection);
    connect(ui->reset_button, &QPushButton::clicked, this, &DiaryEditor::reset_day, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEditor::update_theme,
        Qt::QueuedConnection);
    update_theme();

    // Render current month.
    auto current_date = date;
    change_month(current_date);

    // The diary editor DOES NOT USE internal_diary_changed. It implements its own change detection system!!!
}

DiaryEditor::~DiaryEditor()
{
    delete ui;
    delete save_shortcut;
}

void DiaryEditor::update_theme()
{
    // When this function is run in the constructor, no buttons should exist yet.
    emit sig_re_render_buttons(
        td::CalendarButtonData{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt});
}

DiaryCalendarButton *DiaryEditor::create_button(const td::CalendarButtonData &&d)
{
    auto ptr = new DiaryCalendarButton(d);

    connect(this, &DiaryEditor::sig_re_render_buttons, ptr, &DiaryCalendarButton::re_render, Qt::QueuedConnection);
    connect(ptr, &DiaryCalendarButton::sig_clicked, this, &DiaryEditor::date_clicked, Qt::QueuedConnection);

    return ptr;
}

// Note for future me and any other readers:
// There is probably some way to make this function more readable but really this was a write once and never touch again
// thing because of how hard it is to keep track of all the changing variables.
void DiaryEditor::render_month(QDate const &date, std::optional<td::YearMap::iterator> const &opt)
{
    // Note that the YearMap iterator actually contains a month map which holds all the days of the month.
    auto irl_current_day = QDate::currentDate();

    // Render spacers for first row padding.
    for (int i = 0; i < current_month_offset; ++i)
        ui->dates->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum), 0, i, 1, 1);

    int days_added = 0;
    int row = 1;
    int current_row_length = 0;

    // Render rest of first row.
    for (int i = current_month_offset; i < 7; ++i, ++days_added) {
        // Render the day if it has a corresponding entry.
        // There are 2 entirely separate flows here simply because there are 2 checks required to see if the entry is
        // present. I could use goto but that's kind of messy and harder to keep track of compared to plain old copy
        // pasting.

        auto current = irl_current_day == QDate(date.year(), date.month(), i - current_month_offset + 1);

        if (opt) {
            auto const &monthmap = (*opt)->second;
            auto const &entry_iter = monthmap.find(i - current_month_offset + 1);

            if (entry_iter != monthmap.end()) {
                auto const &[important, rating, dummy, d2] = entry_iter->second;

                auto ptr = create_button(td::CalendarButtonData{std::optional(i - current_month_offset + 1),
                    std::optional(important), std::optional(rating), std::optional(false), std::optional(current)});
                ui->dates->addWidget(ptr, 0, i, 1, 1, Qt::AlignCenter);
                continue;
            }
        }

        auto ptr = create_button(td::CalendarButtonData{std::optional(i - current_month_offset + 1),
            std::optional(false), std::optional(td::Rating::Unknown), std::optional(false), std::optional(current)});
        ui->dates->addWidget(ptr, 0, i, 1, 1, Qt::AlignCenter);
    }

    // Render rest of month.
    for (int i = days_added; i < date.daysInMonth(); ++i, ++current_row_length) {
        if (7 == current_row_length) {
            ++row;
            current_row_length = 0;
        }

        // 2 passes required here as well which means 2 kind of same declarations.

        auto current = irl_current_day == QDate(date.year(), date.month(), i + 1);

        if (opt) {
            auto const &monthmap = (*opt)->second;
            auto const &entry_iter = monthmap.find(i + 1);

            if (entry_iter != monthmap.end()) {
                auto const &[important, rating, dummy, d2] = entry_iter->second;

                auto ptr = create_button(td::CalendarButtonData{std::optional(i + 1), std::optional(important),
                    std::optional(rating), std::optional(false), std::optional(current)});
                ui->dates->addWidget(ptr, row, current_row_length, 1, 1, Qt::AlignCenter);
                continue;
            }
        }

        auto ptr = create_button(td::CalendarButtonData{std::optional(i + 1), std::optional(false),
            std::optional(td::Rating::Unknown), std::optional(false), std::optional(current)});
        ui->dates->addWidget(ptr, row, current_row_length, 1, 1, Qt::AlignCenter);
    }

    // Fill in last row with spacers if necessary. There is a vertical spacer beneath the whole grid of buttons to push
    // them up so they don't expand downwards.
    while (current_row_length < 7)
        ui->dates->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum), row, current_row_length++, 1, 1);

    qDebug() << "Rendered month editor (first day of month):" << date;
}

// Returns true if equal, false if not equal.
bool DiaryEditor::compare_snapshots()
{
    // Doesn't compare last edited timestamps.
    return last_entry_snapshot.important == ui->special_box->isChecked() &&
           last_entry_snapshot.rating == static_cast<td::Rating>(ui->rating_dropdown->currentIndex()) &&
           last_entry_snapshot.message == ui->entry_edit->toPlainText().toStdString();
}

void DiaryEditor::change_month(QDate const &date)
{
    auto cb = [this, date](int const res) {
        if (QMessageBox::Cancel == res)
            return;

        if (QMessageBox::Save == res)
            update_day(true);

        // Remove everything from current grid.
        QLayoutItem *child;
        while ((child = ui->dates->takeAt(0)) != 0) {
            delete child->widget();
            delete child;
        }

        QDate const first_day(date.year(), date.month(), 1);
        // dayOfWeek() returns a number from 1 to 7.
        current_month_offset = first_day.dayOfWeek() - 1;
        last_selected_day = date.day();

        // Set the calendar widget UI (not the info pane).
        ui->month_dropdown->blockSignals(true);
        ui->year_edit->blockSignals(true);

        ui->month_dropdown->setCurrentIndex(date.month() - 1);
        ui->year_edit->setDate(date);

        ui->month_dropdown->blockSignals(false);
        ui->year_edit->blockSignals(false);

        // Render current month.
        render_month(first_day, DiaryHolder::instance()->get_monthmap(date));

        // Render current day.
        td::CalendarButtonData const d{
            std::optional(date.day()), std::nullopt, std::nullopt, std::optional(true), std::nullopt};
        render_day(d, true);

        emit InternalManager::instance()->update_data(date);
        qDebug() << "Changed month in editor and broadcasted update_data:" << date;
    };

    if (!compare_snapshots())
        return cmb::confirm_switch_entries(this, cb);

    cb(QMessageBox::Discard);
}

void DiaryEditor::render_day(td::CalendarButtonData const &d, bool const set_info_pane)
{
    // Get x, y coords of button from 1-42.
    int const x = (*d.day + current_month_offset - 1) % 7;
    int const y = static_cast<int>((*d.day + current_month_offset - 1) / 7);

    auto const &button = qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(y, x)->widget());
    button->re_render(d);

    if (set_info_pane) {
        auto const &new_date = QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, *d.day);
        auto const &opt = DiaryHolder::instance()->get_entry(new_date);

        if (opt) {
            last_entry_snapshot = (*opt)->second;
            update_info_pane(new_date, (*opt)->second);
        }
        else {
            auto default_entry = td::Entry{false, td::Rating::Unknown, "", 0};
            last_entry_snapshot = default_entry;
            update_info_pane(new_date, default_entry);
        }
    }
}

void DiaryEditor::next_month()
{
    QDate &&next = ui->year_edit->date().addMonths(1);
    next.setDate(next.year(), next.month(), 1);
    if (next.isValid())
        change_month(next);
}

void DiaryEditor::prev_month()
{
    QDate &&prev = ui->year_edit->date().addMonths(-1);
    prev.setDate(prev.year(), prev.month(), 1);
    if (prev.isValid())
        change_month(prev);
}

void DiaryEditor::month_changed(int const month)
{
    change_month(QDate(ui->year_edit->date().year(), month + 1, 1));
}

void DiaryEditor::year_changed(QDate const &date)
{
    if (date.isValid())
        change_month(date);
}

// Triggered when a calendar button is clicked.
void DiaryEditor::date_clicked(int const day)
{
    ui->alert_text->setText("");
    ui->alert_text->update();

    // Don't respond to spam clicks on same button.
    if (day == last_selected_day)
        return;

    auto cb = [this, day](int const res) {
        if (QMessageBox::Cancel == res)
            return;

        if (QMessageBox::Yes == res)
            update_day(true); // Suppress entry saved message so it doesn't appear on new date.

        td::CalendarButtonData const old{
            std::optional(last_selected_day), std::nullopt, std::nullopt, std::optional(false), std::nullopt};
        td::CalendarButtonData const n{
            std::optional(day), std::nullopt, std::nullopt, std::optional(true), std::nullopt};
        // Remove selected border from old calendar button.
        render_day(old, false);
        // Add selected border to new calendar button.
        render_day(n, true);

        last_selected_day = day;
    };

    if (!compare_snapshots())
        cmb::confirm_switch_entries(this, cb);
    else
        cb(QMessageBox::No);
}

void DiaryEditor::update_info_pane(QDate const &date, td::Entry const &entry)
{
    ui->date_label->setText(QString("%1 %2%3 %4")
                                .arg(date.toString("dddd"), QString::number(date.day()),
                                    misc::get_day_suffix(date.day()), date.toString("MMMM")));
    ui->date_label->update();

    ui->rating_dropdown->blockSignals(true);
    ui->special_box->blockSignals(true);
    ui->entry_edit->blockSignals(true);

    if (0 == entry.last_updated) {
        ui->last_edited->setText("");
        ui->last_edited->setToolTip("Never edited before.");
    }
    else {
        QDateTime last_edited;
        last_edited.setTime_t(entry.last_updated);
        // Thanks stackoverflow ;)
        ui->last_edited->setText("Last edited " + last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
        ui->last_edited->setToolTip(
            last_edited.toString("dddd MMMM d%1 yyyy 'at' h:mm:ss ap").arg(misc::get_day_suffix(date.day())));
    }
    ui->last_edited->update();

    ui->rating_dropdown->setCurrentIndex(static_cast<int>(entry.rating));
    ui->special_box->setChecked(entry.important);
    ui->entry_edit->setPlainText(entry.message.data());

    ui->rating_dropdown->blockSignals(false);
    ui->special_box->blockSignals(false);
    ui->entry_edit->blockSignals(false);

    qDebug() << "Updated info pane:" << date;
}

// The suppress_error parameter stops the save error box from popping up.
// This is useful when locking the diary as typically the user is AFK.
// The app shouldn't get stuck on the save error dialog as then it wouldn't be able to lock itself properly.
void DiaryEditor::update_day(bool const suppress_error)
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    // Add the entry to the in memory map.
    auto const &new_date =
        QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, last_selected_day);
    td::Entry const e{ui->special_box->isChecked(), static_cast<td::Rating>(ui->rating_dropdown->currentIndex()),
        ui->entry_edit->toPlainText().toStdString(), QDateTime::currentSecsSinceEpoch()};

    DiaryHolder::instance()->create_entry(new_date, e);

    // Actually try and save the diary.
    if (!DiaryHolder::instance()->save() && suppress_error)
        return cmb::display_local_diary_save_error(this);

    td::CalendarButtonData const d{std::optional(last_selected_day), std::optional(ui->special_box->isChecked()),
        std::optional(static_cast<td::Rating>(ui->rating_dropdown->currentIndex())), std::nullopt, std::nullopt};
    // This updates the day button in the calendar widget.
    render_day(d, false);

    // The last edited field is the only one that needs updating in this instance.
    QDateTime last_edited;
    last_edited.setTime_t(e.last_updated);
    // Thanks stackoverflow ;)
    ui->last_edited->setText("Last edited " + last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
    ui->last_edited->update();
    ui->alert_text->setText("Updated entry.");
    ui->alert_text->update();

    // This updates the information in the other tabs.
    // The pixels tab should call end_busy_mode when it is done re rendering.
    InternalManager::instance()->update_data(new_date);
    qDebug() << "Updated entry and broadcasted update_data:" << new_date;
}

void DiaryEditor::delete_day()
{
    auto cb = [this](int const res) {
        if (QMessageBox::No == res)
            return;

        InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);

        // Remove the entry from the in memory map.
        auto const &new_date =
            QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, last_selected_day);
        DiaryHolder::instance()->delete_entry(new_date);

        // Actually try and save the diary.
        if (!DiaryHolder::instance()->save())
            return cmb::display_local_diary_save_error(this);

        ui->alert_text->setText("Deleted entry.");
        ui->alert_text->update();

        update_info_pane(new_date, td::Entry{false, td::Rating::Unknown, "", 0});

        td::CalendarButtonData const d{std::optional(last_selected_day), std::optional(false),
            std::optional(static_cast<td::Rating>(td::Rating::Unknown)), std::nullopt, std::nullopt};
        render_day(d, false);

        // This updates the information in the other tabs.
        // The pixels tab should call end_busy_mode when it is done re rendering.
        InternalManager::instance()->update_data(new_date);
        qDebug() << "Deleted entry and broadcasted update_data:" << new_date;
    };

    // Shift click bypasses confirmation dialog.
    if (Qt::ShiftModifier == QGuiApplication::keyboardModifiers())
        return cb(QMessageBox::Yes);

    auto msgbox = new QMessageBox(this);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("Are you sure you want to delete this entry?");
    msgbox->setInformativeText("This action is not reversible!");
    msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox->setDefaultButton(QMessageBox::No);
    QObject::connect(msgbox, &QMessageBox::finished, cb);
    msgbox->show();
}

void DiaryEditor::reset_day()
{
    auto current_date = QDate::currentDate();
    change_month(current_date);
}
