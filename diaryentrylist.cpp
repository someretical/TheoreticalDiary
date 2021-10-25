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

#include "diaryentrylist.h"
#include "diaryholder.h"
#include "diarywindow.h"
#include "theoreticaldiary.h"
#include "ui_diaryentrylist.h"

#include <QAction>
#include <QDate>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSpacerItem>
#include <QString>
#include <QWidget>
#include <optional>
#include <sstream>
#include <string>

#define LONGEST_LINE_LENGTH 100

DiaryEntryList::DiaryEntryList(QWidget *parent)
    : QDialog(parent), ui(new Ui::DiaryEntryList) {
  ui->setupUi(this);

  first_created = new QDate(*qobject_cast<DiaryWindow *>(parent)->current_date);

  // Load styles
  QFile file(":/styles/day_label_base.qss");
  file.open(QIODevice::ReadOnly);
  s_base = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_very_bad.qss");
  file.open(QIODevice::ReadOnly);
  s_very_bad = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_bad.qss");
  file.open(QIODevice::ReadOnly);
  s_bad = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_ok.qss");
  file.open(QIODevice::ReadOnly);
  s_ok = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_good.qss");
  file.open(QIODevice::ReadOnly);
  s_good = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_very_good.qss");
  file.open(QIODevice::ReadOnly);
  s_very_good = new QString(file.readAll());
  file.close();

  file.setFileName(":/styles/day_label_starred.qss");
  file.open(QIODevice::ReadOnly);
  s_starred = new QString(file.readAll());
  file.close();

  // Setup actions
  auto action = findChild<QAction *>("action_next_month");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryEntryList::next_month);

  action = findChild<QAction *>("action_last_month");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryEntryList::last_month);

  action = findChild<QAction *>("action_reset");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryEntryList::reset_month);

  connect(ui->month_chooser,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DiaryEntryList::dropdown_changed);
  connect(ui->year_edit, &QDateEdit::dateChanged, this,
          &DiaryEntryList::date_changed);

  reset_month();
}

DiaryEntryList::~DiaryEntryList() {
  delete ui;
  delete first_created;
  delete s_base;
  delete s_very_bad;
  delete s_ok;
  delete s_good;
  delete s_very_good;
  delete s_starred;
}

void DiaryEntryList::rerender_current_month() {
  change_month(QDate(ui->year_edit->date()));
}

void DiaryEntryList::reset_month() {
  change_month(*first_created);

  ui->scroll_area->widget()->adjustSize();
  qApp->processEvents();
  ui->scroll_area->verticalScrollBar()->triggerAction(
      QAbstractSlider::SliderToMaximum);
}

QLabel *DiaryEntryList::create_day_label(
    std::pair<const int, const td::Entry> const i) {
  auto day = new QLabel(QString::number(i.second.day), this);

  QFont f = day->font();
  f.setPointSize(14);
  day->setFont(f);
  day->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  // Setting minimum to fixed causes the label to align in the vertical centre
  // if the message is expanded. EVEN THOUGH THE ALIGNMENT ABOVE SPECIFICALLY
  // STATES ALIGN TOP.
  // I give up making this work.
  day->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  // Set colour scheme
  QString stylesheet;
  stylesheet.append(s_base);

  switch (i.second.rating) {
  case td::Rating::VeryBad:
    stylesheet.append(s_very_bad);
    break;
  case td::Rating::Bad:
    stylesheet.append(s_bad);
    break;
  case td::Rating::Ok:
    stylesheet.append(s_ok);
    break;
  case td::Rating::Good:
    stylesheet.append(s_good);
    break;
  case td::Rating::VeryGood:
    stylesheet.append(s_very_good);
    break;
  }

  // Give border if starred
  if (i.second.important)
    stylesheet.append(s_starred);

  day->setStyleSheet(stylesheet);

  return day;
}

void DiaryEntryList::render_month(std::optional<td::EntryMap *> entries) {
  if (!entries) {
    auto label =
        new QLabel("It seems there are no entries yet for this month...", this);
    QFont f = label->font();
    f.setPointSize(14);
    label->setFont(f);
    label->setAlignment(Qt::AlignCenter);

    return ui->dates->addWidget(label, 0, 0);
  }

  int row_counter = 0;
  for (const auto &i : **entries) {
    // Don't add any days that don't have text entries
    if (i.second.message.empty())
      continue;

    auto message = new ClickableLabel(i.second.day, i.second.message, this);

    ui->dates->addWidget(create_day_label(i), row_counter, 0);
    ui->dates->addWidget(message, row_counter++, 1);
  }
}

void DiaryEntryList::change_month(const QDate date) {
  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->dates->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  // Update the selector UI
  ui->month_chooser->blockSignals(true);
  ui->year_edit->blockSignals(true);

  ui->month_chooser->setCurrentIndex(date.month() - 1);
  ui->year_edit->setDate(date);

  ui->month_chooser->blockSignals(false);
  ui->year_edit->blockSignals(false);

  // Get list of entries
  auto year_map = &TheoreticalDiary::instance()->diary_holder->diary->years;
  auto year_iter = year_map->find(ui->year_edit->date().year());

  if (year_iter == year_map->end()) {
    render_month(std::nullopt);
  } else {
    auto month_map = &year_iter->second.months;
    auto month_iter = month_map->find(ui->year_edit->date().month());

    if (month_iter == month_map->end()) {
      render_month(std::nullopt);
    } else {
      render_month(
          std::make_optional<td::EntryMap *>(&month_iter->second.days));
    }
  }

  ui->scroll_area->verticalScrollBar()->setValue(0);
}

// Month dropdown menu
void DiaryEntryList::dropdown_changed(const int index) {
  change_month(QDate(ui->year_edit->date().year(), index + 1, 1));
}

// Next month button
void DiaryEntryList::next_month() {
  QDate next = ui->year_edit->date();
  next = QDate(next.year(), next.month() + 1, 1);
  if (next.isValid()) {
    change_month(next);
    ui->scroll_area->verticalScrollBar()->setValue(0);
  }
}

// Previous month button
void DiaryEntryList::last_month() {
  QDate prev = ui->year_edit->date();
  prev = QDate(prev.year(), prev.month() - 1, 1);
  if (prev.isValid()) {
    change_month(prev);
    // This is needed to make sure the scroll bar actually hits the bottom
    ui->scroll_area->widget()->adjustSize();
    qApp->processEvents();

    ui->scroll_area->verticalScrollBar()->triggerAction(
        QAbstractSlider::SliderToMaximum);
  }
}

// For year edit box
void DiaryEntryList::date_changed(const QDate &date) {
  if (date.isValid())
    change_month(date);
}

//
//
// Clickable Label
ClickableLabel::ClickableLabel(const int &d, const std::string &t,
                               QWidget *parent)
    : QLabel(parent) {
  day = new int(d);
  full_text = new std::string(t);
  expanded = new bool(false);

  // Set text
  std::string truncated;
  ClickableLabel::get_trunc_first_line(t, truncated);
  setText(QString::fromStdString(truncated));

  QFont f = font();
  f.setPointSize(14);
  setFont(f);

  setAlignment(Qt::AlignTop);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  setCursor(QCursor(Qt::PointingHandCursor));
}

ClickableLabel::~ClickableLabel() {
  delete day;
  delete full_text;
  delete expanded;
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
  if (*expanded) {
    std::string truncated;
    ClickableLabel::get_trunc_first_line(*full_text, truncated);
    setText(QString::fromStdString(truncated));
  } else {
    setText(QString::fromStdString(*full_text));
  }

  *expanded = !*expanded;
}

void ClickableLabel::get_trunc_first_line(std::string const &input,
                                          std::string &res) {
  // Get first line
  // https://stackoverflow.com/a/5059112
  std::istringstream f(input);
  std::string second_line_exists;
  std::getline(f, res);

  // Truncate line and append ... if it's too long
  if (LONGEST_LINE_LENGTH < res.size()) {
    res.resize(LONGEST_LINE_LENGTH);

    res.append("...");
  } else if (std::getline(f, second_line_exists)) {
    // There can be multiple lines in an entry where the first line does not
    // have at least LONGEST_LINE_LENGTH characters.
    res.append("...");
  }
}
