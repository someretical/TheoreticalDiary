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
#include "../core/theoreticaldiary.h"
#include "diarymenu.h"
#include "ui_diaryentryviewer.h"

#include <sstream>

const int LONGEST_LINE_LENGTH = 110;
const int DAY_LABEL_SIZE = 50;

DiaryEntryViewer::DiaryEntryViewer(const DiaryEditor *editor, QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryEntryViewer) {
  ui->setupUi(this);

  current_month = new QDate(*qobject_cast<DiaryMenu *>(parent)->first_created);
  rating_stylesheets = new std::vector<QString>();
  black_star = new QString("");
  white_star = new QString("");

  // Navigator slots
  connect(ui->month_dropdown,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DiaryEntryViewer::month_changed, Qt::QueuedConnection);
  connect(ui->year_edit, &QDateEdit::dateChanged, this,
          &DiaryEntryViewer::year_changed, Qt::QueuedConnection);
  connect(ui->next_month, &QPushButton::clicked, this,
          &DiaryEntryViewer::next_month, Qt::QueuedConnection);
  connect(ui->prev_month, &QPushButton::clicked, this,
          &DiaryEntryViewer::prev_month, Qt::QueuedConnection);

  connect(editor, &DiaryEditor::sig_re_render, this,
          &DiaryEntryViewer::change_month, Qt::QueuedConnection);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryEntryViewer::apply_theme, Qt::QueuedConnection);
  apply_theme();

  change_month(*current_month, true);
  // Make scroll bar hit the bottom
  QTimer::singleShot(0, this, [&]() {
    ui->scrollArea->widget()->adjustSize();
    ui->scrollArea->widget()->update();
    ui->scrollArea->verticalScrollBar()->triggerAction(
        QAbstractSlider::SliderToMaximum);
  });
}

DiaryEntryViewer::~DiaryEntryViewer() {
  delete ui;
  delete current_month;
  delete rating_stylesheets;
  delete black_star;
  delete white_star;
}

void DiaryEntryViewer::apply_theme() {
  const auto theme = TheoreticalDiary::instance()->theme();

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
  *white_star = file.readAll();
  file.close();

  file.setFileName(":/global/black_star.qss");
  file.open(QIODevice::ReadOnly);
  *black_star = file.readAll();
  file.close();

  rating_stylesheets->clear();
  for (int i = 0; i < 6; ++i) {
    file.setFileName(
        QString(":/%1/diary_entry_list/%2.qss").arg(theme, QString::number(i)));
    file.open(QIODevice::ReadOnly);
    rating_stylesheets->push_back(file.readAll());
    file.close();
  }

  emit sig_re_render_theme();
}

void DiaryEntryViewer::change_month(const QDate &date,
                                    const bool ignore_month_check) {
  if (!ignore_month_check) {
    if (current_month->year() == date.year() &&
        current_month->month() == date.month()) {
      return;
    }
  }

  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->entry_grid->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  // Update the selector UI
  ui->month_dropdown->blockSignals(true);
  ui->year_edit->blockSignals(true);

  if (date.isValid()) {
    ui->month_dropdown->setCurrentIndex(date.month() - 1);
    ui->year_edit->setDate(date);
  } else {
    ui->month_dropdown->setCurrentIndex(current_month->month() - 1);
    ui->year_edit->setDate(*current_month);
  }

  ui->month_dropdown->blockSignals(false);
  ui->year_edit->blockSignals(false);

  const auto opt = TheoreticalDiary::instance()->diary_holder->get_monthmap(
      date.isValid() ? date : *current_month);
  if (!opt) {
    auto label =
        new QLabel("It seems there are no entries yet for this month...", this);
    label->setAlignment(Qt::AlignCenter);
    auto f = label->font();
    f.setPointSize(11);
    label->setFont(f);

    *current_month = date;
    return ui->entry_grid->addWidget(label);
  }

  int row_counter = 0;
  for (const auto &i : (*opt)->second) {

    // Don't add any days that don't have text entries
    if (i.second.message.empty())
      continue;

    auto day = new DiaryEntryDayLabel(
        td::LabelData{this, i.first, i.second.rating, i.second.important},
        this);
    auto message = new DiaryEntryDayMessage(i.second.message, this);
    auto spacer =
        new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    ui->entry_grid->addWidget(day, row_counter, 0, 1, 1,
                              Qt::AlignTop | Qt::AlignLeft);
    ui->entry_grid->addWidget(message, row_counter, 1, 1, 1,
                              Qt::AlignVCenter | Qt::AlignLeft);
    ui->entry_grid->addItem(spacer, row_counter++, 2);
  }

  *current_month = date;
}

void DiaryEntryViewer::next_month() {
  const QDate next = ui->year_edit->date().addMonths(1);
  if (next.isValid())
    change_month(next, false);
}

void DiaryEntryViewer::prev_month() {
  const QDate prev = ui->year_edit->date().addMonths(-1);
  if (prev.isValid()) {
    change_month(prev, false);

    // Make scroll bar hit bottom
    QTimer::singleShot(0, this, [&]() {
      ui->scrollArea->widget()->adjustSize();
      ui->scrollArea->widget()->update();
      ui->scrollArea->verticalScrollBar()->triggerAction(
          QAbstractSlider::SliderToMaximum);
    });
  }
}

void DiaryEntryViewer::month_changed(const int) {
  change_month(QDate(ui->year_edit->date().year(),
                     ui->month_dropdown->currentIndex() + 1, 1),
               false);
}

void DiaryEntryViewer::year_changed(const QDate &date) {
  if (date.isValid())
    change_month(QDate(ui->year_edit->date().year(),
                       ui->month_dropdown->currentIndex() + 1, 1),
                 false);
}

/*
 * DiaryEntryDayLabel class
 */
DiaryEntryDayLabel::DiaryEntryDayLabel(const td::LabelData &d, QWidget *parent)
    : QLabel(parent) {
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

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryEntryDayLabel::apply_theme, Qt::QueuedConnection);
  connect(d.parent, &DiaryEntryViewer::sig_re_render_theme, this,
          &DiaryEntryDayLabel::apply_theme, Qt::QueuedConnection);
  apply_theme();
}

DiaryEntryDayLabel::~DiaryEntryDayLabel() {}

void DiaryEntryDayLabel::apply_theme() {
  // Set colour theme
  QString stylesheet((*data.parent->rating_stylesheets)[data.rating]);

  // Set background star if necessary
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
DiaryEntryDayMessage::DiaryEntryDayMessage(const std::string &m,
                                           QWidget *parent)
    : QLabel(parent) {
  message = new std::string(m);
  expanded = false;

  // Set text
  std::string truncated;
  DiaryEntryDayMessage::get_trunc_first_line(m, truncated);
  setText(QString::fromStdString(truncated));
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  setTextInteractionFlags(Qt::TextSelectableByMouse |
                          Qt::LinksAccessibleByMouse |
                          Qt::LinksAccessibleByKeyboard);

  QFont f = font();
  f.setPointSize(14);
  setFont(f);

  update();

  setCursor(QCursor(Qt::PointingHandCursor));

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryEntryDayMessage::apply_theme, Qt::QueuedConnection);
  apply_theme();
}

DiaryEntryDayMessage::~DiaryEntryDayMessage() { delete message; }

void DiaryEntryDayMessage::apply_theme() {}

void DiaryEntryDayMessage::mouseDoubleClickEvent(const QMouseEvent *event) {
  if (event->button() != Qt::LeftButton)
    return;

  if (expanded) {
    std::string truncated;
    DiaryEntryDayMessage::get_trunc_first_line(*message, truncated);
    setText(QString::fromStdString(truncated));
  } else {
    setText(QString::fromStdString(*message));
  }

  expanded = !expanded;
}

void DiaryEntryDayMessage::get_trunc_first_line(const std::string &input,
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
