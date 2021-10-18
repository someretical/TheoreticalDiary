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
#include "diarywindow.h"
#include "theoreticaldiary.h"
#include "ui_theoreticalcalendar.h"

#include <QAction>
#include <QDate>
#include <QFile>
#include <QIcon>
#include <QLayoutItem>
#include <QPushButton>
#include <QSpacerItem>
#include <QString>
#include <memory>
#include <random>

TheoreticalCalendar::TheoreticalCalendar(QWidget *parent)
    : QWidget(parent), ui(new Ui::TheoreticalCalendar) {
  ui->setupUi(this);

  first_created = new QDate(QDate::currentDate());

  QFile ss_file(":/styles/theoreticalcalendar.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  // Load star icon
  star_icon = new QIcon(":/images/star.svg");

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
          qobject_cast<DiaryWindow *>(parent), &DiaryWindow::update_info);

  current_month_offset = new int(0);
  last_selected_index = new int(0);
  change_month(0, 0, true);
}

TheoreticalCalendar::~TheoreticalCalendar() {
  delete ui;
  delete first_created;
  delete star_icon;
  delete current_month_offset;
  delete last_selected_index;
  delete s_base;
  delete s_default;
  delete s_very_bad;
  delete s_bad;
  delete s_ok;
  delete s_good;
  delete s_very_good;
  delete s_selected;
}

void TheoreticalCalendar::set_button_stylesheet(CalendarButton &button,
                                                const bool selected) {
  QString stylesheet(*s_base);

  switch (*button.rating) {
  case 0:
    stylesheet.append(s_default);
    break;
  case 1:
    stylesheet.append(s_very_bad);
    break;
  case 2:
    stylesheet.append(s_bad);
    break;
  case 3:
    stylesheet.append(s_ok);
    break;
  case 4:
    stylesheet.append(s_good);
    break;
  case 5:
    stylesheet.append(s_very_good);
    break;
  }

  if (selected) {
    stylesheet.append(s_selected);
  }

  button.setStyleSheet(stylesheet);
}

// Renders a month
void TheoreticalCalendar::change_month(const int year, const int month,
                                       const bool reset) {
  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->dates->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  // Set helper variables
  std::unique_ptr<QDate> first_day(
      new QDate((reset ? first_created->year() : year),
                (reset ? first_created->month() : month), 1));

  // dayOfWeek() returns a number from 1 to 7
  *current_month_offset = first_day->dayOfWeek() - 1;

  ui->month_chooser->blockSignals(true);
  ui->year_edit->blockSignals(true);
  if (reset) {
    // Initialise it now to prevent change_month() from dereferencing a nullptr
    *last_selected_index = first_created->day() - 1 + *current_month_offset;
    // Set the dropdown menu to be the current month
    ui->month_chooser->setCurrentIndex(first_created->month() - 1);
    // Set the year choose to the current year
    ui->year_edit->setDate(*first_created);
  } else {
    *last_selected_index = *current_month_offset;
    ui->month_chooser->setCurrentIndex(first_day->month() - 1);
    ui->year_edit->setDate(*first_day);
  }
  ui->month_chooser->blockSignals(false);
  ui->year_edit->blockSignals(false);

  // Re render given month, select 1st day of month

  // Placeholder RNG to simulate colours
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, 5);

  // Render spacers for first row padding
  for (int i = 0; i < *current_month_offset; ++i) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, 0, i, 1, 1);
  }

  // Render rest of first row
  int days_added = 0;
  for (int i = *current_month_offset; i < 7; ++i, ++days_added) {
    int rating = dist(rng);
    QIcon *star = nullptr;

    auto button = new CalendarButton(
        &TheoreticalCalendar::clicked_on, i - *current_month_offset, rating,
        star, QString::number(i - *current_month_offset + 1), this);
    set_button_stylesheet(*button, false);
    ui->dates->addWidget(button, 0, i, 1, 1);
  }

  // Render main block
  int row = 0 != current_month_offset ? 1 : 0;
  int current_row_length = 0;

  for (int i = days_added; i < first_day->daysInMonth();
       ++i, ++current_row_length) {
    if (7 == current_row_length) {
      ++row;
      current_row_length = 0;
    }

    int rating = dist(rng);
    QIcon *star = star_icon;

    auto button =
        new CalendarButton(&TheoreticalCalendar::clicked_on, i, rating, star,
                           QString::number(i + 1), this);
    set_button_stylesheet(*button, false);
    ui->dates->addWidget(button, row, current_row_length, 1, 1);
  }

  // Fill in last row with spacers
  while (current_row_length < 7) {
    auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum);
    ui->dates->addItem(spacer, row, current_row_length++, 1, 1);
  }

  rerender_day(*last_selected_index, true);
}

void TheoreticalCalendar::rerender_day(const int index, const bool selected) {
  // Get x, y coords of button from 0 - 41 index (7 columns * 6 rows = 42)
  int y = (int)(index / 7);
  int x = index % 7;

  auto button =
      dynamic_cast<CalendarButton *>(ui->dates->itemAtPosition(y, x)->widget());

  // Set the stylesheet for the particular index again
  set_button_stylesheet(*button, selected);

  // Update info
  emit sig_update_info(ui->year_edit->date().year(),
                       ui->month_chooser->currentIndex() + 1,
                       *(button->base_0_day) + 1);
}

void TheoreticalCalendar::clicked_on(const int base_0_day) {
  if (base_0_day + *current_month_offset != *last_selected_index) {
    rerender_day(*last_selected_index, false);
    rerender_day(base_0_day + *current_month_offset, true);

    *last_selected_index = base_0_day + *current_month_offset;
  }
}

void TheoreticalCalendar::dropdown_changed(const int index) {
  change_month(ui->year_edit->date().year(), index + 1);
}

void TheoreticalCalendar::next_month() {
  if (ui->month_chooser->currentIndex() != 11) {
    change_month(ui->year_edit->date().year(),
                 ui->month_chooser->currentIndex() + 2);
  }
}

void TheoreticalCalendar::last_month() {
  if (ui->month_chooser->currentIndex() != 0) {
    change_month(ui->year_edit->date().year(),
                 ui->month_chooser->currentIndex());
  }
}

void TheoreticalCalendar::date_changed(const QDate &date) {
  if (date.isValid()) {
    change_month(date.year(), date.month());
  }
}

// CalendarButton class
CalendarButton::CalendarButton(void (TheoreticalCalendar::*slot)(const int),
                               const int __base_0_day, const int _rating,
                               const QIcon *icon, const QString &base_1_day,
                               QWidget *parent)
    : QPushButton(base_1_day, parent) {
  setFlat(false);
  setAutoDefault(true);

  if (icon != nullptr)
    setIcon(*icon);

  base_0_day = new int(__base_0_day);
  rating = new int(_rating);

  connect(this, &CalendarButton::clicked, this, &CalendarButton::clicked_on);
  connect(this, &CalendarButton::sig_clicked,
          qobject_cast<TheoreticalCalendar *>(parent), slot);
}

CalendarButton::~CalendarButton() {
  delete base_0_day;
  delete rating;
}

void CalendarButton::clicked_on() { emit sig_clicked(*base_0_day); }
