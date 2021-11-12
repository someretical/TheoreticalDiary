/*
 * This file is part of Theoretical Diary.
 *
 * Theoretical Diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Theoretical Diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "diarypixels.h"
#include "../core/theoreticaldiary.h"
#include "diarymenu.h"
#include "ui_diarypixels.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSpacerItem>

const char *MONTH_LETTERS = "JFMAMJJASOND";
const int LABEL_SIZE = 34;

DiaryPixels::DiaryPixels(const DiaryEditor *editor, QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryPixels) {
  ui->setupUi(this);

  rating_stylesheets = new std::vector<QString>();

  current_year = new QDate(QDate::currentDate());
  ui->year_edit->setDate(*current_year);

  connect(editor, &DiaryEditor::sig_re_render, this, &DiaryPixels::render_grid);
  connect(ui->render_button, &QPushButton::clicked, this,
          &DiaryPixels::render_grid);
  connect(ui->export_img_button, &QPushButton::clicked, this,
          &DiaryPixels::export_image);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryPixels::apply_theme);
  apply_theme();

  render_grid();
}

DiaryPixels::~DiaryPixels() {
  delete ui;
  delete current_year;
  delete rating_stylesheets;
}

void DiaryPixels::apply_theme() {
  auto theme = TheoreticalDiary::instance()->theme();

  QFile file(QString(":/%1/diarypixels.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();

  rating_stylesheets->clear();
  for (int i = 0; i < 6; ++i) {
    file.setFileName(
        QString(":/%1/pixels/%2.qss").arg(theme, QString::number(i)));
    file.open(QIODevice::ReadOnly);
    rating_stylesheets->push_back(file.readAll());
    file.close();
  }
}

void DiaryPixels::render_grid() {
  ui->render_button->setEnabled(false);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Remove everything from current grid
  QLayoutItem *child;
  while ((child = ui->grid->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  *current_year = ui->year_edit->date();

  auto opt =
      TheoreticalDiary::instance()->diary_holder->get_yearmap(*current_year);

  // Set new grid
  if (!opt) {
    auto label =
        new QLabel("It seems there are no entries yet for this year...", this);
    auto f = label->font();
    f.setPointSize(11);
    label->setFont(f);

    ui->grid->addWidget(label);

    QApplication::restoreOverrideCursor();
    return;
  }

  auto date = QDate::currentDate();
  auto year = current_year->year();

  for (int month = 0; month < 12; ++month) {
    auto label = new QLabel(QString(MONTH_LETTERS[month]), this);
    QFont f = label->font();
    f.setPointSize(14);
    f.setBold(true);
    label->setFont(f);
    label->setFixedHeight(LABEL_SIZE);
    label->setFixedWidth(LABEL_SIZE);
    label->setAlignment(Qt::AlignCenter);
    ui->grid->addWidget(label, month, 0);

    int days = QDate(current_year->year(), month + 1, 1).daysInMonth();

    // This block runs if the month doesn't exist at all.
    auto iter = (*opt)->second.find(month + 1 /* Month is index 1 based */);
    if (iter == (*opt)->second.end()) {
      for (int day = 0; day < days; ++day) {
        date.setDate(year, month + 1, day + 1);
        ui->grid
            ->addWidget(
                new PixelLabel(td::Rating::Unknown, date, this), month, day + 1 /* +1 here because of the month label added at the start of each row */);
      }

      continue;
    }

    // This code runs if some/all dates in a month exist.
    for (int day = 0; day < days; ++day) {
      auto iter2 = iter->second.find(day + 1 /* day is index 1 based */);

      date.setDate(year, month + 1, day + 1);
      if (iter2 == iter->second.end()) {
        ui->grid->addWidget(new PixelLabel(td::Rating::Unknown, date, this),
                            month, day + 1);
      } else {
        ui->grid->addWidget(new PixelLabel(iter2->second.rating, date, this),
                            month, day + 1);
      }
    }
  }

  ui->render_button->setEnabled(true);
  QApplication::restoreOverrideCursor();
}

void DiaryPixels::export_image() {
  auto filename = QFileDialog::getSaveFileName(
      this, "Export image",
      QString("%1/%2.png")
          .arg(QDir::homePath(), QString::number(ui->year_edit->date().year())),
      "Images (*.png);;All files");

  if (filename.isEmpty())
    return;

  // Thanks to https://stackoverflow.com/a/24341699
  if (ui->hidden_frame->grab().save(filename)) {
    QMessageBox ok(this);
    QPushButton ok_button("OK", &ok);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    ok.setText("Image exported.");
    ok.addButton(&ok_button, QMessageBox::AcceptRole);
    ok.setDefaultButton(&ok_button);
    ok.setTextInteractionFlags(Qt::NoTextInteraction);

    ok.exec();
  } else {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setText("Export failed.");
    rip.setInformativeText(
        "The app could not write to the specified location.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
  }
}

/*
 * PixelLabel class
 */
PixelLabel::PixelLabel(const td::Rating &r, const QDate &date, QWidget *parent)
    : QLabel(parent) {
  setFixedWidth(LABEL_SIZE);
  setFixedHeight(LABEL_SIZE);
  setStyleSheet(qobject_cast<DiaryPixels *>(parent)->rating_stylesheets->at(
      static_cast<int>(r)));
  setToolTip(QString("%1 %2%3").arg(date.toString("MMMM"),
                                    QString::number(date.day()),
                                    DiaryMenu::get_day_suffix(date.day())));
}

PixelLabel::~PixelLabel() {}
