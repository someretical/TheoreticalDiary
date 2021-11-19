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

#include "diaryeditor.h"
#include "../core/theoreticaldiary.h"
#include "diarymenu.h"
#include "mainwindow.h"
#include "ui_diaryeditor.h"

#include <QDateTime>
#include <QFile>
#include <QMessageBox>

DiaryEditor::DiaryEditor(QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryEditor) {
  ui->setupUi(this);
  ui->alert_text->setText("");

  // These will be filled when apply_theme is called.
  base_stylesheet = new QString("");
  selected_stylesheet = new QString("");
  white_star = new QString("");
  black_star = new QString("");
  rating_stylesheets = new std::vector<QString>();

  current_month_offset = 0;
  last_selected_day = 0;

  // Calendar widget actions
  // See https://doc.qt.io/qt-5/qcombobox.html#currentIndexChanged for why
  // QOverload<int> is needed
  connect(ui->month_dropdown,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DiaryEditor::month_changed);
  connect(ui->year_edit, &QDateEdit::dateChanged, this,
          &DiaryEditor::year_changed);
  connect(ui->next_month, &QPushButton::clicked, this,
          &DiaryEditor::next_month);
  connect(ui->prev_month, &QPushButton::clicked, this,
          &DiaryEditor::prev_month);

  // Info pane actions
  connect(ui->update_button, &QPushButton::clicked, this,
          &DiaryEditor::update_day);
  connect(ui->delete_button, &QPushButton::clicked, this,
          &DiaryEditor::delete_day);
  connect(ui->reset_button, &QPushButton::clicked, this,
          &DiaryEditor::reset_day);

  // Trigger unsaved changes
  connect(ui->rating_dropdown,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          TheoreticalDiary::instance(), &TheoreticalDiary::diary_changed);
  connect(ui->special_box, &QCheckBox::clicked, TheoreticalDiary::instance(),
          &TheoreticalDiary::diary_changed);
  connect(ui->entry_edit, &QPlainTextEdit::textChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::diary_changed);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryEditor::apply_theme);
  apply_theme();

  // Render current month
  change_month(*qobject_cast<DiaryMenu *>(parent)->first_created, true);
}

DiaryEditor::~DiaryEditor() {
  delete ui;
  delete rating_stylesheets;
  delete base_stylesheet;
  delete selected_stylesheet;
}

void DiaryEditor::apply_theme() {
  auto theme = TheoreticalDiary::instance()->theme();

  QFile file(QString(":/%1/diaryeditor.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();

  file.setFileName(QString(":/%1/theoretical_calendar/base.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  *base_stylesheet = file.readAll();
  file.close();

  file.setFileName(
      QString(":/%1/theoretical_calendar/selected.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  *selected_stylesheet = file.readAll();
  file.close();

  file.setFileName(":/global/white_star.qss");
  file.open(QIODevice::ReadOnly);
  *white_star = file.readAll();
  file.close();

  file.setFileName(":/global/black_star.qss");
  file.open(QIODevice::ReadOnly);
  *black_star = file.readAll();
  file.close();

  rating_stylesheets->clear();
  for (int i = 0; i < 6; ++i) {
    file.setFileName(QString(":/%1/theoretical_calendar/%2.qss")
                         .arg(theme, QString::number(i)));
    file.open(QIODevice::ReadOnly);
    rating_stylesheets->push_back(file.readAll());
    file.close();
  }

  // When this function is run in the constructor, no buttons should exist yet.
  emit sig_re_render_buttons(td::CalendarButtonData{
      std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt});
}

// Note for future me and any other readers:
// There is probably some way to make this function more readable but really
// this was a write once and never touch again thing because of how hard it is
// to keep track of all the changing variables.
void DiaryEditor::render_month(const QDate &date,
                               const std::optional<td::YearMap::iterator> opt) {
  // Note that the YearMap iterator actually contains a month map which holds
  // all the days of the month

  // Render spacers for first row padding
  for (int i = 0; i < current_month_offset; ++i) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, 0, i, 1, 1);
  }

  int days_added = 0;
  int row = 1;
  int current_row_length = 0;

  // Render rest of first row
  for (int i = current_month_offset; i < 7; ++i, ++days_added) {
    // Render the day if it has a corresponding entry.
    // There are 2 entirely separate flows here simply because there are 2
    // checks required to see if the entry is present. I could use goto but
    // that's kind of messy and harder to keep track of compared to plain old
    // copy pasting.
    if (opt) {
      auto entry_iter = (*opt)->second.find(i - current_month_offset + 1);

      if (entry_iter != (*opt)->second.end()) {
        td::CalendarButtonData d{
            std::make_optional<int>(i - current_month_offset + 1),
            std::make_optional<DiaryEditor *>(this),
            std::make_optional<bool>(entry_iter->second.important),
            std::make_optional<td::Rating>(entry_iter->second.rating),
            std::make_optional<bool>(false)};
        ui->dates->addWidget(new CalendarButton(d), 0, i, 1, 1,
                             Qt::AlignCenter);
        continue;
      }
    }

    td::CalendarButtonData d{
        std::make_optional<int>(i - current_month_offset + 1),
        std::make_optional<DiaryEditor *>(this),
        std::make_optional<bool>(false),
        std::make_optional<td::Rating>(td::Rating::Unknown),
        std::make_optional<bool>(false)};

    ui->dates->addWidget(new CalendarButton(d), 0, i, 1, 1, Qt::AlignCenter);
  }

  // Render rest of month
  for (int i = days_added; i < date.daysInMonth(); ++i, ++current_row_length) {
    if (7 == current_row_length) {
      ++row;
      current_row_length = 0;
    }

    // 2 passes required here as well which means 2 kind of same declarations.
    if (opt) {
      auto entry_iter = (*opt)->second.find(i + 1);

      if (entry_iter != (*opt)->second.end()) {
        td::CalendarButtonData d{
            std::make_optional<int>(i + 1),
            std::make_optional<DiaryEditor *>(this),
            std::make_optional<bool>(entry_iter->second.important),
            std::make_optional<td::Rating>(entry_iter->second.rating),
            std::make_optional<bool>(false)};
        ui->dates->addWidget(new CalendarButton(d), row, current_row_length, 1,
                             1, Qt::AlignCenter);
        continue;
      }
    }

    td::CalendarButtonData d{
        std::make_optional<int>(i + 1), std::make_optional<DiaryEditor *>(this),
        std::make_optional<bool>(false),
        std::make_optional<td::Rating>(td::Rating::Unknown),
        std::make_optional<bool>(false)};

    ui->dates->addWidget(new CalendarButton(d), row, current_row_length, 1, 1,
                         Qt::AlignCenter);
  }

  // Fill in last row with spacers if necessary.
  // There is a vertical spacer beneath the whole grid of buttons to push them
  // up so they don't expand downwards.
  while (current_row_length < 7) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, row, current_row_length++, 1, 1);
  }
}

bool DiaryEditor::confirm_switch() {
  QMessageBox confirm(this);

  QPushButton discard_button("Do not save", &confirm);
  QFont f3 = discard_button.font();
  f3.setPointSize(11);
  discard_button.setFont(f3);
  // This chain is a beast.
  // It goes (MainWindow->central_grid)->Diarymenu (2)
  // (DiaryMenu->ui->tabWidget->editor)->DiaryEditor (4)
  // I THINK
  discard_button.setStyleSheet(*qobject_cast<MainWindow *>(parentWidget()
                                                               ->parentWidget()
                                                               ->parentWidget()
                                                               ->parentWidget()
                                                               ->parentWidget()
                                                               ->parentWidget())
                                    ->danger_button_style);
  QPushButton save_button("Save", &confirm);
  QFont f = save_button.font();
  f.setPointSize(11);
  save_button.setFont(f);
  QPushButton cancel_button("Cancel", &confirm);
  cancel_button.setFlat(true);
  QFont f2 = cancel_button.font();
  f2.setPointSize(11);
  cancel_button.setFont(f2);

  confirm.setText("There are unsaved changes.");
  confirm.setInformativeText(
      "Are you sure you want to switch dates without saving?");
  confirm.addButton(&save_button, QMessageBox::AcceptRole);
  confirm.addButton(&cancel_button, QMessageBox::RejectRole);
  confirm.addButton(&discard_button, QMessageBox::DestructiveRole);
  confirm.setDefaultButton(&save_button);
  confirm.setTextInteractionFlags(Qt::NoTextInteraction);

  switch (confirm.exec()) {
  case QMessageBox::AcceptRole:
    update_day();
    break;
  case QMessageBox::RejectRole:
    return false;
  case QMessageBox::DestructiveRole:
    break;
  }

  TheoreticalDiary::instance()->diary_modified = false;
  return true;
}

void DiaryEditor::change_month(const QDate &date,
                               const bool &suppress_confirm) {
  if (!suppress_confirm && TheoreticalDiary::instance()->diary_modified &&
      !confirm_switch())
    return;

  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->dates->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  QDate first_day(date.year(), date.month(), 1);
  // dayOfWeek() returns a number from 1 to 7
  current_month_offset = first_day.dayOfWeek() - 1;
  last_selected_day = date.day();

  // Set the calendar widget UI (not the info pane)
  ui->month_dropdown->blockSignals(true);
  ui->year_edit->blockSignals(true);

  ui->month_dropdown->setCurrentIndex(date.month() - 1);
  ui->year_edit->setDate(date);

  ui->month_dropdown->blockSignals(false);
  ui->year_edit->blockSignals(false);

  // Render current month
  render_month(first_day,
               TheoreticalDiary::instance()->diary_holder->get_monthmap(date));

  // Render current day
  td::CalendarButtonData d{std::make_optional<int>(date.day()), std::nullopt,
                           std::nullopt, std::nullopt,
                           std::make_optional<bool>(true)};
  render_day(d, true);
}

void DiaryEditor::render_day(const td::CalendarButtonData &d,
                             const bool &set_info_pane) {
  // Get x, y coords of button from 1-42
  int x = (*d.day + current_month_offset - 1) % 7;
  int y = static_cast<int>((*d.day + current_month_offset - 1) / 7);

  auto button =
      qobject_cast<CalendarButton *>(ui->dates->itemAtPosition(y, x)->widget());
  button->re_render(d);

  if (set_info_pane) {
    auto current_date = QDate(ui->year_edit->date().year(),
                              ui->month_dropdown->currentIndex() + 1, *d.day);
    auto opt =
        TheoreticalDiary::instance()->diary_holder->get_entry(current_date);
    update_info_pane(current_date,
                     opt ? (*opt)->second
                         : td::Entry{false, td::Rating::Unknown, "", 0});
  }
}

void DiaryEditor::next_month() {
  QDate next = ui->year_edit->date().addMonths(1);
  next.setDate(next.year(), next.month(), 1);
  if (next.isValid())
    change_month(next, false);
}

void DiaryEditor::prev_month() {
  QDate prev = ui->year_edit->date().addMonths(-1);
  prev.setDate(prev.year(), prev.month(), 1);
  if (prev.isValid())
    change_month(prev, false);
}

void DiaryEditor::month_changed(const int month) {
  change_month(QDate(ui->year_edit->date().year(), month + 1, 1), false);
}

void DiaryEditor::year_changed(const QDate &date) {
  if (date.isValid())
    change_month(date, false);
}

// Triggered when a calendar button is clicked
void DiaryEditor::date_clicked(const int day) {
  ui->alert_text->setText("");
  ui->alert_text->update();

  // Don't respond to spam clicks on same button.
  if (day == last_selected_day)
    return;

  if (TheoreticalDiary::instance()->diary_modified && !confirm_switch())
    return;

  td::CalendarButtonData old{std::make_optional<int>(last_selected_day),
                             std::nullopt, std::nullopt, std::nullopt,
                             std::make_optional<bool>(false)};
  td::CalendarButtonData n{std::make_optional<int>(day), std::nullopt,
                           std::nullopt, std::nullopt,
                           std::make_optional<bool>(true)};
  // Remove selected border from old calendar button
  render_day(old, false);
  // Add selected border to new calendar button
  render_day(n, true);

  last_selected_day = day;
}

void DiaryEditor::update_info_pane(const QDate &date, const td::Entry &entry) {
  ui->date_label->setText(
      QString("%1 %2%3 %4")
          .arg(date.toString("dddd"), QString::number(date.day()),
               DiaryMenu::get_day_suffix(date.day()), date.toString("MMMM")));
  ui->date_label->update();

  ui->rating_dropdown->blockSignals(true);
  ui->special_box->blockSignals(true);
  ui->entry_edit->blockSignals(true);

  if (0 == entry.last_updated) {
    ui->last_edited->setText("");
  } else {
    QDateTime last_edited;
    last_edited.setTime_t(entry.last_updated);
    // Thanks stackoverflow ;)
    ui->last_edited->setText("Last edited " +
                             last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
  }
  ui->last_edited->update();

  ui->rating_dropdown->setCurrentIndex(static_cast<int>(entry.rating));
  ui->special_box->setChecked(entry.important);
  ui->entry_edit->setPlainText(QString::fromStdString(entry.message));

  ui->rating_dropdown->blockSignals(false);
  ui->special_box->blockSignals(false);
  ui->entry_edit->blockSignals(false);
}

void DiaryEditor::update_day() {
  // Add the entry to the in memory map.
  auto current_date =
      QDate(ui->year_edit->date().year(),
            ui->month_dropdown->currentIndex() + 1, last_selected_day);
  td::Entry e{ui->special_box->isChecked(),
              static_cast<td::Rating>(ui->rating_dropdown->currentIndex()),
              ui->entry_edit->toPlainText().toStdString(),
              QDateTime::currentSecsSinceEpoch()};
  TheoreticalDiary::instance()->diary_holder->create_entry(current_date, e);

  // Actually try and save the diary.
  auto res = qobject_cast<MainWindow *>(parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget())
                 ->save_diary(false);
  if (!res)
    return; // The save error window will be shown by the function.

  ui->alert_text->setText("Saved entry.");
  ui->alert_text->update();

  td::CalendarButtonData d{
      std::make_optional<int>(last_selected_day), std::nullopt,
      std::make_optional<bool>(ui->special_box->isChecked()),
      std::make_optional<td::Rating>(
          static_cast<td::Rating>(ui->rating_dropdown->currentIndex())),
      std::nullopt};
  // This updates the day button in the calendar widget.
  render_day(d, false);

  // The last edited field is the only one that needs updating in this instance.
  QDateTime last_edited;
  last_edited.setTime_t(e.last_updated);
  // Thanks stackoverflow ;)
  ui->last_edited->setText("Last edited " +
                           last_edited.toString("dd MMM ''yy 'at' h:mm ap"));
  ui->last_edited->update();

  // This updates the information in the other tabs.
  emit sig_re_render(current_date, true);
}

void DiaryEditor::delete_day() {
  QMessageBox confirm(this);

  QPushButton yes("YES", &confirm);
  QFont f = yes.font();
  f.setPointSize(11);
  yes.setFont(f);
  yes.setStyleSheet(*qobject_cast<MainWindow *>(parentWidget()
                                                    ->parentWidget()
                                                    ->parentWidget()
                                                    ->parentWidget()
                                                    ->parentWidget()
                                                    ->parentWidget())
                         ->danger_button_style);
  QPushButton no("NO", &confirm);
  no.setFlat(true);
  QFont f2 = no.font();
  f2.setPointSize(11);
  no.setFont(f2);

  confirm.setText("Delete entry.");
  confirm.setInformativeText("Are you sure you want to delete this entry?");
  confirm.addButton(&yes, QMessageBox::AcceptRole);
  confirm.addButton(&no, QMessageBox::RejectRole);
  confirm.setDefaultButton(&no);
  confirm.setTextInteractionFlags(Qt::NoTextInteraction);

  if (confirm.exec() != QMessageBox::AcceptRole)
    return;

  // Remove the entry from the in memory map
  auto current_date =
      QDate(ui->year_edit->date().year(),
            ui->month_dropdown->currentIndex() + 1, last_selected_day);
  TheoreticalDiary::instance()->diary_holder->delete_entry(current_date);

  // Actually try and save the diary.
  auto res = qobject_cast<MainWindow *>(parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget()
                                            ->parentWidget())
                 ->save_diary(false);
  if (!res)
    return; // The save error window will be shown by the function.

  ui->alert_text->setText("Deleted entry.");
  ui->alert_text->update();

  update_info_pane(current_date, td::Entry{false, td::Rating::Unknown, "", 0});

  td::CalendarButtonData d{std::make_optional<int>(last_selected_day),
                           std::nullopt, std::make_optional<bool>(false),
                           std::make_optional<td::Rating>(
                               static_cast<td::Rating>(td::Rating::Unknown)),
                           std::nullopt};
  render_day(d, false);

  emit sig_re_render(current_date, true);
}

void DiaryEditor::reset_day() {
  change_month(
      *qobject_cast<DiaryMenu *>(
           parentWidget()->parentWidget()->parentWidget()->parentWidget())
           ->first_created,
      false);
}
