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
#include "mainwindow.h"
#include "ui_diaryeditor.h"

DiaryEditor::DiaryEditor(QDate const &date, QWidget *parent) : QWidget(parent), ui(new Ui::DiaryEditor)
{
    ui->setupUi(this);
    ui->alert_text->setText("");
    ui->alert_text->update();
    setup_buttons();

    current_month_offset = 0;
    current_date = date;
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
    connect(MainWindow::instance(), &MainWindow::sig_update_diary, this, &DiaryEditor::update_day);
    connect(ui->update_button, &QPushButton::clicked, this, &DiaryEditor::update_day, Qt::QueuedConnection);
    connect(ui->delete_button, &QPushButton::clicked, this, &DiaryEditor::delete_day, Qt::QueuedConnection);
    connect(ui->reset_button, &QPushButton::clicked, this, &DiaryEditor::reset_day, Qt::QueuedConnection);

    // Render current month.
    change_month(current_date);

    // The diary editor DOES NOT USE internal_diary_changed. It implements its own change detection system!!!
}

DiaryEditor::~DiaryEditor()
{
    delete ui;
    delete save_shortcut;
}

void DiaryEditor::setup_buttons()
{
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            auto ptr = new DiaryCalendarButton(td::CalendarButtonData{std::optional(-1), std::optional(false),
                std::optional(td::Rating::Unknown), std::optional(false), std::optional(false)});
            connect(ptr, &DiaryCalendarButton::sig_clicked, this, &DiaryEditor::date_clicked, Qt::QueuedConnection);
            ui->dates->addWidget(ptr, row, col, 1, 1, Qt::AlignCenter);
        }
    }
}

void DiaryEditor::render_month(std::optional<td::YearMap::iterator> const &opt)
{
    auto irl_current_day = QDate::currentDate();
    int counter = 1;

    if (!opt) {
        // Render the first row.
        for (int day = 0; day < 7; ++day) {
            auto button = qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(0, day)->widget());

            if (day < current_month_offset) {
                button->setVisible(false);
            }
            else {
                auto current_cond = irl_current_day == QDate(current_date.year(), current_date.month(), counter);
                button->render(td::CalendarButtonData{std::optional(counter++), std::optional(false),
                    std::optional(td::Rating::Unknown), std::optional(false), std::optional(current_cond)});
                button->setVisible(true);
            }
        }

        // Render the rest of the rows.
        for (int row = 1; row < 6; ++row) {
            for (int col = 0; col < 7; ++col) {
                auto button = qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(row, col)->widget());

                if (counter > current_date.daysInMonth()) {
                    button->setVisible(false);
                }
                else {
                    auto current_cond = irl_current_day == QDate(current_date.year(), current_date.month(), counter);
                    button->render(td::CalendarButtonData{std::optional(counter++), std::optional(false),
                        std::optional(td::Rating::Unknown), std::optional(false), std::optional(current_cond)});
                    button->setVisible(true);
                }
            }
        }

        return;
    }

    auto const &monthmap = (*opt)->second;

    // Render the first row.
    for (int day = 0; day < 7; ++day) {
        auto button = qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(0, day)->widget());

        if (day < current_month_offset) {
            button->setVisible(false);
        }
        else {
            auto const &entry_iter = monthmap.find(counter);
            auto current_cond = irl_current_day == QDate(current_date.year(), current_date.month(), counter);

            if (entry_iter == monthmap.end()) {
                button->render(td::CalendarButtonData{std::optional(counter++), std::optional(false),
                    std::optional(td::Rating::Unknown), std::optional(false), std::optional(current_cond)});
            }
            else {
                auto const &[important, rating, dummy, d2] = entry_iter->second;

                button->render(td::CalendarButtonData{std::optional(counter++), std::optional(important),
                    std::optional(rating), std::optional(false), std::optional(current_cond)});
            }

            button->setVisible(true);
        }
    }

    // Render the rest of the rows.
    for (int row = 1; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            auto button = qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(row, col)->widget());

            if (counter > current_date.daysInMonth()) {
                button->setVisible(false);
            }
            else {
                auto const &entry_iter = monthmap.find(counter);
                auto current_cond = irl_current_day == QDate(current_date.year(), current_date.month(), counter);

                if (entry_iter == monthmap.end()) {
                    button->render(td::CalendarButtonData{std::optional(counter++), std::optional(false),
                        std::optional(td::Rating::Unknown), std::optional(false), std::optional(current_cond)});
                }
                else {
                    auto const &[important, rating, dummy, d2] = entry_iter->second;

                    button->render(td::CalendarButtonData{std::optional(counter++), std::optional(important),
                        std::optional(rating), std::optional(false), std::optional(current_cond)});
                }

                button->setVisible(true);
            }
        }
    }

    qDebug() << "Rendered month for current date:" << current_date;
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

        if (QMessageBox::Save == res && !update_day())
            return;

        current_date = date;
        QDate const first_day(date.year(), date.month(), 1);
        // dayOfWeek() returns a number from 1 to 7.
        current_month_offset = first_day.dayOfWeek() - 1;

        // Set the calendar widget UI (not the info pane).
        ui->month_dropdown->blockSignals(true);
        ui->year_edit->blockSignals(true);

        ui->month_dropdown->setCurrentIndex(date.month() - 1);
        ui->year_edit->setDate(date);

        ui->month_dropdown->blockSignals(false);
        ui->year_edit->blockSignals(false);

        // Render current month.
        render_month(DiaryHolder::instance()->get_monthmap(date));

        // Render current day with selection outline.
        // This is not done within the render_month function since not just the rendering has to be done.
        td::CalendarButtonData const d{
            std::optional(date.day()), std::nullopt, std::nullopt, std::optional(true), std::nullopt};
        render_day(d, true);

        qDebug() << "Changed month in editor and broadcasting update_data:" << date;
        emit InternalManager::instance()->update_data(date);
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

    qobject_cast<DiaryCalendarButton *>(ui->dates->itemAtPosition(y, x)->widget())->render(d);

    if (!set_info_pane)
        return;

    auto const &new_date = QDate(current_date.year(), current_date.month(), *d.day);
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

void DiaryEditor::next_month()
{
    QDate &&next = current_date.addMonths(1);
    next.setDate(next.year(), next.month(), 1);
    if (next.isValid())
        change_month(next);
}

void DiaryEditor::prev_month()
{
    QDate &&prev = current_date.addMonths(-1);
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
    if (day == current_date.day())
        return;

    auto cb = [this, day](int const res) {
        if (QMessageBox::Cancel == res)
            return;

        if (QMessageBox::Yes == res && !update_day(false))
            return;

        td::CalendarButtonData const old{
            std::optional(current_date.day()), std::nullopt, std::nullopt, std::optional(false), std::nullopt};
        td::CalendarButtonData const n{
            std::optional(day), std::nullopt, std::nullopt, std::optional(true), std::nullopt};
        // Remove selected border from old calendar button.
        render_day(old, false);
        // Add selected border to new calendar button.
        render_day(n, true);

        current_date = QDate(current_date.year(), current_date.month(), day);
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
    //    ui->entry_edit->setPlainText("PLACEHOLDER");

    ui->rating_dropdown->blockSignals(false);
    ui->special_box->blockSignals(false);
    ui->entry_edit->blockSignals(false);

    qDebug() << "Updated info pane:" << date;
}

// The suppress_error parameter stops the save error box from popping up.
// This is useful when locking the diary as typically the user is AFK.
// The app shouldn't get stuck on the save error dialog as then it wouldn't be able to lock itself properly.
bool DiaryEditor::update_day(bool const suppress_error)
{
    AppBusyLock lock;

    // Add the entry to the in memory map.
    td::Entry const e{ui->special_box->isChecked(), static_cast<td::Rating>(ui->rating_dropdown->currentIndex()),
        ui->entry_edit->toPlainText().toStdString(), QDateTime::currentSecsSinceEpoch()};

    DiaryHolder::instance()->create_entry(current_date, e);

    // Actually try and save the diary.
    if (!DiaryHolder::instance()->save() && suppress_error) {
        cmb::display_local_diary_save_error(this);
        return false;
    }

    td::CalendarButtonData const d{std::optional(current_date.day()), std::optional(ui->special_box->isChecked()),
        std::optional(static_cast<td::Rating>(ui->rating_dropdown->currentIndex())), std::optional(true), std::nullopt};
    // This updates the day button in the calendar widget.
    render_day(d, false);
    last_entry_snapshot = e;

    // The last edited field is the only one that needs updating in this instance.
    QDateTime last_edited;
    last_edited.setTime_t(e.last_updated);
    // Thanks stackoverflow ;)
    ui->last_edited->setText("Last edited " + last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
    ui->last_edited->setToolTip(
        last_edited.toString("dddd MMMM d%1 yyyy 'at' h:mm:ss ap").arg(misc::get_day_suffix(current_date.day())));
    ui->last_edited->update();
    ui->alert_text->setText("Updated entry.");
    ui->alert_text->update();

    InternalManager::instance()->update_data(current_date);
    qDebug() << "Updated entry and broadcasted update_data:" << current_date;
    return true;
}

void DiaryEditor::delete_day()
{
    auto cb = [this](int const res) {
        if (QMessageBox::No == res)
            return;

        AppBusyLock lock;

        // Remove the entry from the in memory map.
        DiaryHolder::instance()->delete_entry(current_date);

        // Actually try and save the diary.
        if (!DiaryHolder::instance()->save())
            return cmb::display_local_diary_save_error(this);

        ui->alert_text->setText("Deleted entry.");
        ui->alert_text->update();

        render_day(td::CalendarButtonData{std::optional(current_date.day()), std::optional(false),
                       std::optional(td::Rating::Unknown), std::optional(true), std::nullopt},
            false);
        auto empty = td::Entry{false, td::Rating::Unknown, "", 0};
        update_info_pane(current_date, empty);
        last_entry_snapshot = empty;

        InternalManager::instance()->update_data(current_date);
        qDebug() << "Deleted entry and broadcasted update_data:" << current_date;
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
