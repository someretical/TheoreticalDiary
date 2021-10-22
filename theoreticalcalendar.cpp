/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "theoreticalcalendar.h"
#include "changepanealert.h"
#include "diaryholder.h"
#include "diarywindow.h"
#include "theoreticaldiary.h"
#include "ui_theoreticalcalendar.h"

#include <QAction>
#include <QCursor>
#include <QDate>
#include <QFile>
#include <QIcon>
#include <QLayoutItem>
#include <QPushButton>
#include <QSpacerItem>
#include <QString>
#include <algorithm>
#include <memory>

TheoreticalCalendar::TheoreticalCalendar(QWidget *parent)
    : QWidget(parent), ui(new Ui::TheoreticalCalendar) {
  ui->setupUi(this);

  p = new DiaryWindow *(qobject_cast<DiaryWindow *>(parent));
  first_created =
      new QDate((**p).current_date->year(), (**p).current_date->month(),
                (**p).current_date->day());
  current_month_offset = new int(0);
  last_selected_day = new int(0);

  QFile ss_file(":/styles/theoreticalcalendar.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  // Different rated days have different stylesheets
  QFile _s_base(":/styles/s_base.qss");
  _s_base.open(QIODevice::ReadOnly);
  s_base = new QString(_s_base.readAll());

  QFile _s_default(":/styles/s_default.qss");
  _s_default.open(QIODevice::ReadOnly);
  s_default = new QString(_s_default.readAll());

  QFile _s_very_bad(":/styles/s_very_bad.qss");
  _s_very_bad.open(QIODevice::ReadOnly);
  s_very_bad = new QString(_s_very_bad.readAll());

  QFile _s_bad(":/styles/s_bad.qss");
  _s_bad.open(QIODevice::ReadOnly);
  s_bad = new QString(_s_bad.readAll());

  QFile _s_ok(":/styles/s_ok.qss");
  _s_ok.open(QIODevice::ReadOnly);
  s_ok = new QString(_s_ok.readAll());

  QFile _s_good(":/styles/s_good.qss");
  _s_good.open(QIODevice::ReadOnly);
  s_good = new QString(_s_good.readAll());

  QFile _s_very_good(":/styles/s_very_good.qss");
  _s_very_good.open(QIODevice::ReadOnly);
  s_very_good = new QString(_s_very_good.readAll());

  QFile _s_selected(":/styles/s_selected.qss");
  _s_selected.open(QIODevice::ReadOnly);
  s_selected = new QString(_s_selected.readAll());

  QFile _s_star_white(":/styles/s_star_white.qss");
  _s_star_white.open(QIODevice::ReadOnly);
  s_star_white = new QString(_s_star_white.readAll());

  QFile _s_star_black(":/styles/s_star_black.qss");
  _s_star_black.open(QIODevice::ReadOnly);
  s_star_black = new QString(_s_star_black.readAll());

  auto action = findChild<QAction *>("action_next_month");
  addAction(action);
  connect(action, &QAction::triggered, this, &TheoreticalCalendar::next_month);

  action = findChild<QAction *>("action_last_month");
  addAction(action);
  connect(action, &QAction::triggered, this, &TheoreticalCalendar::last_month);

  connect(ui->month_chooser,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &TheoreticalCalendar::dropdown_changed);
  connect(ui->year_edit, &QDateEdit::dateChanged, this,
          &TheoreticalCalendar::date_changed);

  connect(this, &TheoreticalCalendar::sig_update_info,
          qobject_cast<DiaryWindow *>(parent), &DiaryWindow::update_info_pane);

  // Calling change_month below will reset the invalid date set here. This is
  // done to specifically bypass the check that occurs in
  // DiaryWindow::update_pane_info that compares the dates of p->current_date
  // and first_created so no uncessary resources are spent searching if the user
  // clicks on the same button loads of times.
  (**p).current_date->setDate(0, 0, 0);
  change_month(*first_created);
}

TheoreticalCalendar::~TheoreticalCalendar() {
  delete p;
  delete ui;
  delete first_created;
  delete current_month_offset;
  delete last_selected_day;
  delete s_base;
  delete s_star_white;
  delete s_star_black;
  delete s_default;
  delete s_very_bad;
  delete s_bad;
  delete s_ok;
  delete s_good;
  delete s_very_good;
  delete s_selected;
}

void TheoreticalCalendar::render_month(
    const QDate &first_day, const std::optional<td::EntryMap *> &map) {
  int days_added = 0;
  int row = 1;
  int current_row_length = 0;

  for (int i = *current_month_offset; i < 7; ++i, ++days_added) {
    if (map.has_value()) {
      auto entry_iter = (*map)->find(i - *current_month_offset + 1);

      if (entry_iter != (*map)->end()) {
        td::CalendarButtonData d{
            i - *current_month_offset + 1,
            std::make_optional<TheoreticalCalendar *>(this),
            std::make_optional<bool>(entry_iter->second.important),
            std::make_optional<td::Rating>(entry_iter->second.rating),
            std::make_optional<bool>(false)};
        ui->dates->addWidget(new CalendarButton(d), 0, i, 1, 1);
        continue;
      }
    }

    td::CalendarButtonData d{
        i - *current_month_offset + 1,
        std::make_optional<TheoreticalCalendar *>(this),
        std::make_optional<bool>(false),
        std::make_optional<td::Rating>(td::Rating::Unknown),
        std::make_optional<bool>(false)};

    ui->dates->addWidget(new CalendarButton(d), 0, i, 1, 1);
  }

  for (int i = days_added; i < first_day.daysInMonth();
       ++i, ++current_row_length) {
    if (7 == current_row_length) {
      ++row;
      current_row_length = 0;
    }

    if (map.has_value()) {
      auto entry_iter = (*map)->find(i + 1);

      if (entry_iter != (*map)->end()) {
        td::CalendarButtonData d{
            i + 1, std::make_optional<TheoreticalCalendar *>(this),
            std::make_optional<bool>(entry_iter->second.important),
            std::make_optional<td::Rating>(entry_iter->second.rating),
            std::make_optional<bool>(false)};
        ui->dates->addWidget(new CalendarButton(d), row, current_row_length, 1,
                             1);
        continue;
      }
    }

    td::CalendarButtonData d{
        i + 1, std::make_optional<TheoreticalCalendar *>(this),
        std::make_optional<bool>(false),
        std::make_optional<td::Rating>(td::Rating::Unknown),
        std::make_optional<bool>(false)};

    ui->dates->addWidget(new CalendarButton(d), row, current_row_length, 1, 1);
  }

  while (current_row_length < 7) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, row, current_row_length++, 1, 1);
  }
}

void TheoreticalCalendar::change_month(const QDate date) {
  if (*(**p).current_date_changed) {
    ChangePaneAlert w(this);

    if (w.exec() != QDialog::Accepted)
      return;

    *(**p).current_date_changed = false;
  }

  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->dates->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  QDate first_day(date.year(), date.month(), 1);
  // dayOfWeek() returns a number from 1 to 7
  *current_month_offset = first_day.dayOfWeek() - 1;
  *last_selected_day = date.day();

  // Set the calendar widget UI (not the info pane)
  ui->month_chooser->blockSignals(true);
  ui->year_edit->blockSignals(true);

  ui->month_chooser->setCurrentIndex(date.month() - 1);
  ui->year_edit->setDate(date);

  ui->month_chooser->blockSignals(false);
  ui->year_edit->blockSignals(false);

  // Render spacers for first row padding
  for (int i = 0; i < *current_month_offset; ++i) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, 0, i, 1, 1);
  }

  // Get iterators
  auto year_map = &TheoreticalDiary::instance()->diary_holder->diary->years;
  auto year_iter = year_map->find(ui->year_edit->date().year());

  if (year_iter == year_map->end()) {
    render_month(first_day, std::nullopt);
  } else {
    auto month_map = &year_iter->second.months;
    auto month_iter = month_map->find(ui->year_edit->date().month());

    if (month_iter == month_map->end()) {
      render_month(first_day, std::nullopt);
    } else {
      render_month(first_day, std::make_optional<td::EntryMap *>(
                                  &month_iter->second.days));
    }
  }

  td::CalendarButtonData d{date.day(), std::nullopt, std::nullopt, std::nullopt,
                           std::make_optional<bool>(true)};
  rerender_day(d);
}

void TheoreticalCalendar::rerender_day(const td::CalendarButtonData d) {
  // Get x, y coords of button from 1-42
  int x = (d.day + *current_month_offset - 1) % 7;
  int y = (int)((d.day + *current_month_offset - 1) / 7);

  auto button =
      qobject_cast<CalendarButton *>(ui->dates->itemAtPosition(y, x)->widget());
  button->update_aesthetic(d);

  // For the diary window
  emit sig_update_info(QDate(ui->year_edit->date().year(),
                             ui->year_edit->date().month(), button->data->day));
}

// Whenever an actual tile is clicked
void TheoreticalCalendar::clicked_on(const int day) {
  // Don't respond to spam clicks on same button.
  if (day == *last_selected_day)
    return;

  if (*(**p).current_date_changed) {
    ChangePaneAlert w(this);

    if (w.exec() != QDialog::Accepted)
      return;

    *(**p).current_date_changed = false;
  }

  td::CalendarButtonData old{*last_selected_day, std::nullopt, std::nullopt,
                             std::nullopt, std::make_optional<bool>(false)};
  td::CalendarButtonData n{day, std::nullopt, std::nullopt, std::nullopt,
                           std::make_optional<bool>(true)};
  rerender_day(old);
  rerender_day(n);

  *last_selected_day = day;
}

// Month dropdown menu
void TheoreticalCalendar::dropdown_changed(const int index) {
  change_month(QDate(ui->year_edit->date().year(), index + 1, 1));
}

// Next month button
void TheoreticalCalendar::next_month() {
  QDate next = ui->year_edit->date();
  next = QDate(next.year(), next.month() + 1, 1);
  if (next.isValid())
    change_month(next);
}

// Previous month button
void TheoreticalCalendar::last_month() {
  QDate prev = ui->year_edit->date();
  prev = QDate(prev.year(), prev.month() - 1, 1);
  if (prev.isValid())
    change_month(prev);
}

// For year edit box
void TheoreticalCalendar::date_changed(const QDate &date) {
  if (date.isValid())
    change_month(date);
}

//
//
// CalendarButton class constructor
CalendarButton::CalendarButton(const td::CalendarButtonData &d)
    : QPushButton(qobject_cast<QWidget *>(*d.parent)) {
  // Sanity check
  if (!d.parent || !d.important || !d.rating)
    throw;

  setAutoDefault(true);
  setCursor(QCursor(Qt::PointingHandCursor));

  data = new td::CalendarButtonData();
  *data = d;
  setText(QString::number(data->day));

  // Link click event through here to pass day variable through.
  connect(this, &CalendarButton::clicked, this, &CalendarButton::clicked_on);
  connect(this, &CalendarButton::sig_clicked, *d.parent,
          &TheoreticalCalendar::clicked_on);

  update_aesthetic(d);
}

CalendarButton::~CalendarButton() { delete data; }

// Update the the following properties of the button:
// - Background colour
// - Icon
// - Whether it is selected or not
void CalendarButton::update_aesthetic(const td::CalendarButtonData &d) {
  // Set stylesheet (determines colours)
  QString stylesheet(*(*data->parent)->s_base);

  // Set background star if necessary
  data->important =
      std::make_optional<bool>(d.important.value_or(*data->important));
  data->rating =
      std::make_optional<td::Rating>(d.rating.value_or(*data->rating));

  if (true == *data->important && td::Rating::Unknown != *data->rating) {
    if (td::Rating::VeryBad == *data->rating ||
        td::Rating::VeryGood == *data->rating) {
      stylesheet.append(*(*data->parent)->s_star_white);
    } else {
      stylesheet.append(*(*data->parent)->s_star_black);
    }
  }

  // Set colour scheme
  auto r = d.rating.value_or(*data->rating);
  data->rating = std::make_optional<td::Rating>(r);
  switch (r) {
  case td::Rating::Unknown:
    stylesheet.append((*data->parent)->s_default);
    break;
  case td::Rating::VeryBad:
    stylesheet.append((*data->parent)->s_very_bad);
    break;
  case td::Rating::Bad:
    stylesheet.append((*data->parent)->s_bad);
    break;
  case td::Rating::Ok:
    stylesheet.append((*data->parent)->s_ok);
    break;
  case td::Rating::Good:
    stylesheet.append((*data->parent)->s_good);
    break;
  case td::Rating::VeryGood:
    stylesheet.append((*data->parent)->s_very_good);
    break;
  }

  // Give border if selected
  data->selected =
      std::make_optional<bool>(d.selected.value_or(*data->selected));
  if (*data->selected) {
    stylesheet.append((*data->parent)->s_selected);
  }

  setStyleSheet(stylesheet);
}

void CalendarButton::clicked_on() { emit sig_clicked(data->day); }
