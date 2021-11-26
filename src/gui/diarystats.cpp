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

#include "diarystats.h"
#include "../core/diaryholder.h"
#include "../core/theoreticaldiary.h"
#include "diarymenu.h"
#include "ui_diarystats.h"

#include <QtCharts/QPolarChart>
#include <QtCharts>

using namespace QtCharts;

const char *ratings[] = {"Unknown", "Very bad", "Bad",
                         "OK",      "Good",     "Very good"};
const qreal angular_min = 1;
// const qreal angular_max = 31;
const qreal radial_min = static_cast<int>(td::Rating::Unknown);
const qreal radial_max = static_cast<int>(td::Rating::VeryGood);
const QColor light_grey =
    QColor(49, 54, 59, 255 /* background colour */).lighter();
const QColor colours[] = {QColor(84, 110, 122, 51), QColor(123, 31, 162, 255),
                          QColor(94, 53, 177, 255), QColor(25, 118, 210, 255),
                          QColor(0, 151, 167, 255), QColor(76, 175, 80, 255)};

DiaryStats::DiaryStats(const DiaryEditor *editor, QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryStats) {
  ui->setupUi(this);

  // Initilise pie chart
  QChart *pie_chart = new QChart();
  pie_chart->setMargins(QMargins(0, 0, 0, 0));
  pie_chart->setBackgroundVisible(false);
  pie_chart->legend()->hide();
  ui->pie_chart_view->setRenderHint(QPainter::Antialiasing);
  ui->pie_chart_view->setChart(pie_chart);

  // Initialise polar chart
  QPolarChart *polar_chart = new QPolarChart();
  polar_chart->setMargins(QMargins(0, 0, 0, 0));
  polar_chart->setBackgroundVisible(false);
  polar_chart->legend()->hide();
  ui->polar_chart_view->setRenderHint(QPainter::Antialiasing);
  ui->polar_chart_view->setChart(polar_chart);

  // Initialise spline chart
  QChart *spline_chart = new QChart();
  spline_chart->setMargins(QMargins(0, 0, 0, 0));
  spline_chart->setBackgroundVisible(false);
  spline_chart->legend()->hide();
  ui->spline_chart_view->setRenderHint(QPainter::Antialiasing);
  ui->spline_chart_view->setChart(spline_chart);

  current_month = new QDate(*qobject_cast<DiaryMenu *>(parent)->first_created);

  connect(editor, &DiaryEditor::sig_re_render, this, &DiaryStats::render_stats,
          Qt::QueuedConnection);

  // Navigator slots
  connect(ui->month_dropdown,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DiaryStats::month_changed, Qt::QueuedConnection);
  connect(ui->year_edit, &QDateEdit::dateChanged, this,
          &DiaryStats::year_changed, Qt::QueuedConnection);
  connect(ui->next_month, &QPushButton::clicked, this, &DiaryStats::next_month,
          Qt::QueuedConnection);
  connect(ui->prev_month, &QPushButton::clicked, this, &DiaryStats::prev_month,
          Qt::QueuedConnection);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryStats::apply_theme, Qt::QueuedConnection);
  apply_theme();

  render_stats(*current_month, true);
}

DiaryStats::~DiaryStats() {
  delete ui;
  delete current_month;
}

void DiaryStats::apply_theme() {
  auto theme = TheoreticalDiary::instance()->theme();

  QFile file(QString(":/global/diarystats.qss"));
  file.open(QIODevice::ReadOnly);
  QString stylesheet(file.readAll());
  file.close();

  file.setFileName(QString(":/%1/diarystats.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  stylesheet.append(file.readAll());
  file.close();

  setStyleSheet(stylesheet);
}

void DiaryStats::next_month() {
  QDate next = ui->year_edit->date().addMonths(1);
  if (next.isValid())
    render_stats(next, false);
}

void DiaryStats::prev_month() {
  QDate prev = ui->year_edit->date().addMonths(-1);
  if (prev.isValid())
    render_stats(prev, false);
}

void DiaryStats::month_changed(const int month) {
  render_stats(QDate(ui->year_edit->date().year(),
                     ui->month_dropdown->currentIndex() + 1, 1),
               false);
}

void DiaryStats::year_changed(const QDate &date) {
  if (date.isValid())
    render_stats(QDate(ui->year_edit->date().year(),
                       ui->month_dropdown->currentIndex() + 1, 1),
                 false);
}

void DiaryStats::render_pie_chart(const std::vector<int> &rating_counts) {
  // Clear all data from the chart so new values can be added.
  ui->pie_chart_view->chart()->removeAllSeries();

  QPieSeries *pie_series = new QPieSeries();
  pie_series->setPieEndAngle(330);

  if (current_month->daysInMonth() == rating_counts[0]) {
    // If there is no data for the current month, display an empty pie chart.
    pie_series->append(ratings[0], current_month->daysInMonth());
    QPieSlice *slice = pie_series->slices().at(0);
    slice->setBrush(colours[0]);
    slice->setLabelColor(QColor(Qt::white));
    slice->setLabelVisible();
    slice->setBorderWidth(4);
    slice->setBorderColor(light_grey);
  } else {
    // Display pie chart slices from largest to smallest.
    auto sorted = std::vector<std::pair<td::Rating, int>>();
    for (int i = 1; i < 6; ++i)
      if (0 != rating_counts[i])
        sorted.push_back(
            std::make_pair(static_cast<td::Rating>(i), rating_counts[i]));

    std::sort(sorted.begin(), sorted.end(),
              [](std::pair<td::Rating, int> a, std::pair<td::Rating, int> b) {
                return a.second > b.second;
              });

    for (const auto &i : sorted) {
      pie_series->append(QString("%1 - %2 day%3")
                             .arg(QString(ratings[static_cast<int>(i.first)]),
                                  QString::number(i.second),
                                  1 == i.second ? "" : "s"),
                         i.second);

      QPieSlice *slice =
          pie_series->slices().at(pie_series->slices().size() - 1);
      slice->setBorderWidth(4);
      slice->setBorderColor(QColor(49, 54, 59, 255));
      slice->setLabelColor(QColor(Qt::white));
      slice->setLabelVisible();
      slice->setBrush(colours[static_cast<int>(i.first)]);
    }
  }

  ui->pie_chart_view->chart()->addSeries(pie_series);
  ui->pie_chart_view->update();
}

void DiaryStats::render_polar_chart(
    const std::optional<td::YearMap::iterator> opt) {
  auto chart = qobject_cast<QPolarChart *>(ui->polar_chart_view->chart());
  const auto angular_max = current_month->daysInMonth();

  chart->removeAllSeries();
  for (const auto i : chart->axes()) {
    chart->removeAxis(i);
  }

  QValueAxis *angular_axis = new QValueAxis();
  angular_axis->setTickCount(angular_max + 1);
  angular_axis->setLabelFormat("%d");
  angular_axis->setLabelsColor(QColor(Qt::white));
  angular_axis->setRange(angular_min, angular_max + 1);
  angular_axis->setGridLineColor(light_grey);
  chart->addAxis(angular_axis, QPolarChart::PolarOrientationAngular);

  QValueAxis *radial_axis = new QValueAxis();
  radial_axis->setTickCount(static_cast<int>(td::Rating::VeryGood) + 1);
  radial_axis->setLabelsVisible(false);
  radial_axis->setRange(radial_min, radial_max);
  radial_axis->setGridLineColor(light_grey);
  chart->addAxis(radial_axis, QPolarChart::PolarOrientationRadial);

  if (opt) {
    for (const auto &i : (*opt)->second) {
      if (td::Rating::Unknown != i.second.rating) {
        QLineSeries *upper = new QLineSeries();
        upper->append(i.first, static_cast<int>(i.second.rating));
        upper->append(i.first + 1, static_cast<int>(i.second.rating));

        QLineSeries *lower = new QLineSeries();
        lower->append(i.first, 0);
        lower->append(i.first + 1, 1); // For some reason this has to be a 1.

        QAreaSeries *area = new QAreaSeries();
        chart->addSeries(area);
        area->setLowerSeries(lower);
        area->setUpperSeries(upper);
        area->setOpacity(0.75);
        area->setColor(colours[static_cast<int>(i.second.rating)]);
        area->setBorderColor(QColor(Qt::transparent));
        area->attachAxis(angular_axis);
        area->attachAxis(radial_axis);
      }
    }
  }

  chart->update();
}

void DiaryStats::render_spline_chart(
    const std::optional<td::YearMap::iterator> opt) {
  auto chart = ui->spline_chart_view->chart();

  chart->removeAllSeries();
  for (const auto i : chart->axes()) {
    chart->removeAxis(i);
  }

  auto x_axis = new QValueAxis();
  chart->addAxis(x_axis, Qt::AlignBottom);
  x_axis->setRange(1, current_month->daysInMonth());
  x_axis->setTickCount(15);
  x_axis->setGridLineColor(light_grey);
  x_axis->setLabelsColor(QColor(Qt::white));
  x_axis->setLabelsVisible();
  x_axis->setTitleText("Day");
  x_axis->setTitleBrush(QBrush(QColor(Qt::white)));

  auto y_axis = new QValueAxis();
  chart->addAxis(y_axis, Qt::AlignLeft);
  y_axis->setRange(1, 5);
  y_axis->setTickCount(5);
  y_axis->setGridLineColor(light_grey);
  y_axis->setLabelFormat("%d");
  y_axis->setLabelsColor(QColor(Qt::white));
  y_axis->setLabelsVisible();
  y_axis->setTitleText("Rating");
  y_axis->setTitleBrush(QBrush(QColor(Qt::white)));

  QSplineSeries *spline_series = new QSplineSeries();
  chart->addSeries(spline_series);
  spline_series->attachAxis(x_axis);
  spline_series->attachAxis(y_axis);
  spline_series->setColor(colours[3]);

  QScatterSeries *scatter_series = new QScatterSeries();
  chart->addSeries(scatter_series);
  scatter_series->attachAxis(x_axis);
  scatter_series->attachAxis(y_axis);
  scatter_series->setMarkerSize(8);
  scatter_series->setColor(QColor(Qt::white));
  scatter_series->setBorderColor(QColor(Qt::transparent));

  if (opt) {
    for (const auto &i : (*opt)->second) {
      if (td::Rating::Unknown != i.second.rating) {
        spline_series->append(i.first, static_cast<int>(i.second.rating));
        scatter_series->append(i.first, static_cast<int>(i.second.rating));
      }
    }
  }

  chart->update();
}

void DiaryStats::render_comparison(const std::vector<int> &rating_counts) {
  ui->t0->setText(QString::number(rating_counts[0]));
  ui->t1->setText(QString::number(rating_counts[1]));
  ui->t2->setText(QString::number(rating_counts[2]));
  ui->t3->setText(QString::number(rating_counts[3]));
  ui->t4->setText(QString::number(rating_counts[4]));
  ui->t5->setText(QString::number(rating_counts[5]));
  ui->ts->setText(QString::number(rating_counts[6]));

  auto prev_month = current_month->addMonths(-1);
  if (!prev_month.isValid()) {
    ui->l0->setText("N/A");
    ui->l1->setText("N/A");
    ui->l2->setText("N/A");
    ui->l3->setText("N/A");
    ui->l4->setText("N/A");
    ui->l5->setText("N/A");
    ui->ls->setText("N/A");

    ui->d0->setText("N/A");
    ui->d1->setText("N/A");
    ui->d2->setText("N/A");
    ui->d3->setText("N/A");
    ui->d4->setText("N/A");
    ui->d5->setText("N/A");
    ui->ds->setText("N/A");
    return;
  }

  auto prev_stats = DiaryStats::get_rating_stats(
      TheoreticalDiary::instance()->diary_holder->get_monthmap(prev_month),
      prev_month.daysInMonth());

  ui->l0->setText(QString::number(prev_stats[0]));
  ui->l1->setText(QString::number(prev_stats[1]));
  ui->l2->setText(QString::number(prev_stats[2]));
  ui->l3->setText(QString::number(prev_stats[3]));
  ui->l4->setText(QString::number(prev_stats[4]));
  ui->l5->setText(QString::number(prev_stats[5]));
  ui->ls->setText(QString::number(prev_stats[6]));

  ui->d0->setText(QString::number(rating_counts[0] - prev_stats[0]));
  ui->d1->setText(QString::number(rating_counts[1] - prev_stats[1]));
  ui->d2->setText(QString::number(rating_counts[2] - prev_stats[2]));
  ui->d3->setText(QString::number(rating_counts[3] - prev_stats[3]));
  ui->d4->setText(QString::number(rating_counts[4] - prev_stats[4]));
  ui->d5->setText(QString::number(rating_counts[5] - prev_stats[5]));
  ui->ds->setText(QString::number(rating_counts[6] - prev_stats[6]));
}

std::vector<int>
DiaryStats::get_rating_stats(const std::optional<td::YearMap::iterator> opt,
                             const int &total_days) {
  auto rating_counts = std::vector<int>{total_days, 0, 0, 0, 0, 0, 0};

  if (opt) {
    for (const auto &i : (*opt)->second) {
      if (td::Rating::Unknown != i.second.rating) {
        --rating_counts[0];
        ++rating_counts[static_cast<int>(i.second.rating)];

        if (i.second.important)
          ++rating_counts[6];
      }
    }
  }

  return rating_counts;
}

void DiaryStats::render_stats(const QDate &date,
                              const bool &ignore_month_check) {
  if (!ignore_month_check) {
    if (current_month->year() == date.year() &&
        current_month->month() == date.month()) {
      return;
    }
  }

  *current_month = date;

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

  auto opt =
      TheoreticalDiary::instance()->diary_holder->get_monthmap(*current_month);
  auto rating_counts =
      DiaryStats::get_rating_stats(opt, current_month->daysInMonth());

  render_pie_chart(rating_counts);
  render_polar_chart(opt);
  render_spline_chart(opt);
  render_comparison(rating_counts);
}
