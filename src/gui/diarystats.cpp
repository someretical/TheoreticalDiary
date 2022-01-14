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

#include "diarystats.h"
#include "../core/diaryholder.h"
#include "ui_diarystats.h"

using namespace QtCharts;

char const *ratings[] = {"Unknown", "Very bad", "Bad", "OK", "Good", "Very good"};
QColor const rating_colours[] = {QColor(84, 110, 122, 51), QColor(123, 31, 162, 255), QColor(94, 53, 177, 255),
    QColor(25, 118, 210, 255), QColor(0, 151, 167, 255), QColor(76, 175, 80, 255)};

qreal const angular_min = 1;
// qreal const angular_max = 31;
qreal const radial_min = static_cast<int>(td::Rating::Unknown);
qreal const radial_max = static_cast<int>(td::Rating::VeryGood);

auto const dark_background = QColor(49, 54, 59, 170);
auto const light_background = QColor(230, 230, 230, 170);
auto const dark_white = light_background.darker();
auto const light_black = dark_background.lighter();

auto const almost_white = QColor(245, 245, 245, 255);
auto const almost_black = QColor(60, 60, 60, 255);

DiaryStats::DiaryStats(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryStats)
{
    ui->setupUi(this);

    // Initilise pie chart.
    auto pie_chart = new QChart();
    pie_chart->setMargins(QMargins(0, 0, 0, 0));
    pie_chart->setBackgroundVisible(false);
    pie_chart->legend()->hide();
    pie_chart->setEnabled(false); // Disables scrolling (at the cost of what?).
    ui->pie_chart_view->setRenderHint(QPainter::Antialiasing);
    ui->pie_chart_view->setChart(pie_chart);

    // Initialise polar chart.
    auto polar_chart = new QPolarChart();
    polar_chart->setMargins(QMargins(0, 0, 0, 0));
    polar_chart->setBackgroundVisible(false);
    polar_chart->legend()->hide();
    polar_chart->setEnabled(false);
    ui->polar_chart_view->setRenderHint(QPainter::Antialiasing);
    ui->polar_chart_view->setChart(polar_chart);

    // Initialise spline chart.
    auto spline_chart = new QChart();
    spline_chart->setMargins(QMargins(0, 0, 0, 0));
    spline_chart->setBackgroundVisible(false);
    spline_chart->legend()->hide();
    spline_chart->setEnabled(false);
    ui->spline_chart_view->setRenderHint(QPainter::Antialiasing);
    ui->spline_chart_view->setChart(spline_chart);

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryStats::render_stats,
        Qt::QueuedConnection);

    // Navigator slots.
    connect(ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DiaryStats::month_changed,
        Qt::QueuedConnection);
    connect(ui->year_edit, &QDateEdit::dateChanged, this, &DiaryStats::year_changed, Qt::QueuedConnection);
    connect(ui->next_month, &QPushButton::clicked, this, &DiaryStats::next_month, Qt::QueuedConnection);
    connect(ui->prev_month, &QPushButton::clicked, this, &DiaryStats::prev_month, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryStats::update_theme,
        Qt::QueuedConnection);
    update_theme();

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryStats::~DiaryStats()
{
    delete ui;
}

void DiaryStats::update_theme()
{
    auto const &theme = InternalManager::instance()->get_theme_str();

    QFile file(QString(":/global/diarystats.qss"));
    file.open(QIODevice::ReadOnly);
    QString stylesheet(file.readAll());
    file.close();

    file.setFileName(QString(":/%1/diarystats.qss").arg(theme));
    file.open(QIODevice::ReadOnly);
    stylesheet.append(file.readAll());

    setStyleSheet(stylesheet);

    render_stats(QDate::currentDate());
}

void DiaryStats::next_month()
{
    auto const &&next = ui->year_edit->date().addMonths(1);
    if (next.isValid())
        render_stats(next);
}

void DiaryStats::prev_month()
{
    auto const &&prev = ui->year_edit->date().addMonths(-1);
    if (prev.isValid())
        render_stats(prev);
}

void DiaryStats::month_changed(int const)
{
    render_stats(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryStats::year_changed(QDate const &date)
{
    if (date.isValid())
        render_stats(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryStats::render_pie_chart(std::vector<int> const &rating_counts)
{
    auto theme = InternalManager::instance()->get_theme();
    // Clear all data from the chart so new values can be added.
    ui->pie_chart_view->chart()->removeAllSeries();

    auto *pie_series = new QPieSeries();
    pie_series->setPieEndAngle(330);

    if (current_date.daysInMonth() == rating_counts[0]) {
        // If there is no data for the current month, display an empty pie chart.
        pie_series->append(ratings[0], current_date.daysInMonth());
        auto slice = pie_series->slices().at(0);
        slice->setBrush(rating_colours[0]);
        slice->setLabelColor(td::Theme::Dark == theme ? almost_white : almost_black);
        slice->setLabelVisible();
        slice->setBorderWidth(4);
        slice->setBorderColor(td::Theme::Dark == theme ? light_black : dark_white);
    }
    else {
        // Display pie chart slices from largest to smallest.
        auto sorted = std::vector<std::pair<td::Rating, int>>();
        for (int i = 1; i < 6; ++i)
            if (0 != rating_counts[i])
                sorted.push_back(std::pair(static_cast<td::Rating>(i), rating_counts[i]));

        std::sort(sorted.begin(), sorted.end(), [](auto const &a, auto const &b) { return a.second > b.second; });

        for (auto const &[rating, count] : sorted) {
            pie_series->append(
                QString("%1 - %2 day%3")
                    .arg(QString(ratings[static_cast<int>(rating)]), QString::number(count), 1 == count ? "" : "s"),
                count);

            auto slice = pie_series->slices().at(pie_series->slices().size() - 1);
            slice->setBorderWidth(4);
            slice->setBorderColor(td::Theme::Dark == theme ? light_black : dark_white);
            slice->setLabelColor(td::Theme::Dark == theme ? almost_white : almost_black);
            slice->setLabelVisible();
            slice->setBrush(rating_colours[static_cast<int>(rating)]);
        }
    }

    ui->pie_chart_view->chart()->addSeries(pie_series);
    ui->pie_chart_view->update();
    qDebug() << "Rendered pie chart.";
}

void DiaryStats::render_polar_chart(std::optional<td::YearMap::iterator> const &opt)
{
    auto theme = InternalManager::instance()->get_theme();
    auto chart = qobject_cast<QPolarChart *>(ui->polar_chart_view->chart());
    auto const angular_max = current_date.daysInMonth();

    chart->removeAllSeries();
    for (auto const &i : chart->axes())
        chart->removeAxis(i);

    auto angular_axis = new QValueAxis();
    angular_axis->setTickCount(angular_max + 1);
    angular_axis->setLabelFormat("%d");
    angular_axis->setLabelsColor(td::Theme::Dark == theme ? almost_white : almost_black);
    angular_axis->setRange(angular_min, angular_max + 1);
    angular_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    chart->addAxis(angular_axis, QPolarChart::PolarOrientationAngular);

    auto radial_axis = new QValueAxis();
    radial_axis->setTickCount(static_cast<int>(td::Rating::VeryGood) + 1);
    radial_axis->setLabelsVisible(false);
    radial_axis->setRange(radial_min, radial_max);
    radial_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    chart->addAxis(radial_axis, QPolarChart::PolarOrientationRadial);

    if (opt) {
        for (auto const &[day, data] : (*opt)->second) {
            auto const &[dummy, rating, d2, d3] = data;

            if (td::Rating::Unknown != rating) {
                auto upper = new QLineSeries();
                upper->append(day, static_cast<int>(rating));
                upper->append(day + 1, static_cast<int>(rating));

                auto lower = new QLineSeries();
                lower->append(day, 0);
                lower->append(day + 1, 1); // For some reason this has to be a 1.

                auto area = new QAreaSeries();
                chart->addSeries(area);
                area->setLowerSeries(lower);
                area->setUpperSeries(upper);
                area->setOpacity(0.75);
                area->setColor(rating_colours[static_cast<int>(rating)]);
                area->setBorderColor(QColor(Qt::transparent));
                area->attachAxis(angular_axis);
                area->attachAxis(radial_axis);
            }
        }
    }

    chart->update();
    qDebug() << "Rendered polar chart.";
}

void DiaryStats::render_spline_chart(std::optional<td::YearMap::iterator> const &opt)
{
    auto theme = InternalManager::instance()->get_theme();
    auto chart = ui->spline_chart_view->chart();

    chart->removeAllSeries();
    for (auto const &i : chart->axes())
        chart->removeAxis(i);

    auto x_axis = new QValueAxis();
    chart->addAxis(x_axis, Qt::AlignBottom);
    x_axis->setRange(1, current_date.daysInMonth());
    x_axis->setTickCount(15);
    x_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    x_axis->setLabelsColor(td::Theme::Dark == theme ? almost_white : almost_black);
    x_axis->setLabelsVisible();
    x_axis->setTitleText("Day");
    x_axis->setTitleBrush(QBrush(td::Theme::Dark == theme ? almost_white : almost_black));

    auto y_axis = new QValueAxis();
    chart->addAxis(y_axis, Qt::AlignLeft);
    y_axis->setRange(1, 5);
    y_axis->setTickCount(5);
    y_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    y_axis->setLabelFormat("%d");
    y_axis->setLabelsColor(td::Theme::Dark == theme ? almost_white : almost_black);
    y_axis->setLabelsVisible();
    y_axis->setTitleText("Rating");
    y_axis->setTitleBrush(QBrush(td::Theme::Dark == theme ? almost_white : almost_black));

    auto scatter_series = new QScatterSeries();
    chart->addSeries(scatter_series);
    scatter_series->attachAxis(x_axis);
    scatter_series->attachAxis(y_axis);
    scatter_series->setMarkerSize(8);
    scatter_series->setColor(td::Theme::Dark == theme ? almost_white : almost_black);
    scatter_series->setBorderColor(QColor(Qt::transparent));

    if (opt) {
        auto prev_day = 0;
        auto current_spline_series = new QSplineSeries();
        chart->addSeries(current_spline_series);
        current_spline_series->attachAxis(x_axis);
        current_spline_series->attachAxis(y_axis);
        current_spline_series->setColor(rating_colours[3]);

        for (auto const &[day, data] : (*opt)->second) {
            auto const &[important, rating, dummy, d2] = data;
            if (td::Rating::Unknown != rating) {
                // Each spline series covers a segment made from consecutive days with known ratings. This is done to
                // prevent the warping of the spline line if there are large gaps between days with ratings. Map
                // elements are ordered so there is no problem just iterating over a map.

                if (prev_day != day - 1 && day != 1) {
                    // Consecutive day chain broken so add the current spline series to the chart and create a new
                    // spline series. The chart takes ownership of the spline series after it is added.
                    prev_day = day;

                    current_spline_series = new QSplineSeries();
                    chart->addSeries(current_spline_series);
                    current_spline_series->attachAxis(x_axis);
                    current_spline_series->attachAxis(y_axis);
                    current_spline_series->setColor(rating_colours[3]);
                }
                else {
                    ++prev_day;
                }

                current_spline_series->append(day, static_cast<int>(rating));

                scatter_series->append(day, static_cast<int>(rating));
            }
        }
    }

    chart->update();
    qDebug() << "Rendered spline chart.";
}

void DiaryStats::render_comparison(std::vector<int> const &rating_counts)
{
    ui->t0->setText(QString::number(rating_counts[0]));
    ui->t1->setText(QString::number(rating_counts[1]));
    ui->t2->setText(QString::number(rating_counts[2]));
    ui->t3->setText(QString::number(rating_counts[3]));
    ui->t4->setText(QString::number(rating_counts[4]));
    ui->t5->setText(QString::number(rating_counts[5]));
    ui->ts->setText(QString::number(rating_counts[6]));

    auto const &prev_month = current_date.addMonths(-1);
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

    auto const &prev_stats =
        DiaryStats::get_rating_stats(DiaryHolder::instance()->get_monthmap(prev_month), prev_month.daysInMonth());

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

    qDebug() << "Rendered stats table.";
}

std::vector<int> DiaryStats::get_rating_stats(std::optional<td::YearMap::iterator> const &opt, int const total_days)
{
    auto rating_counts = std::vector<int>{total_days, 0, 0, 0, 0, 0, 0};

    if (opt) {
        for (auto const &[day, data] : (*opt)->second) {
            auto const &[important, rating, dummy, d2] = data;
            if (td::Rating::Unknown != rating) {
                --rating_counts[0];
                ++rating_counts[static_cast<int>(rating)];

                if (important)
                    ++rating_counts[6];
            }
        }
    }

    return rating_counts;
}

void DiaryStats::render_stats(QDate const &date)
{
    current_date = date;

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

    auto const &opt = DiaryHolder::instance()->get_monthmap(current_date);
    auto const &rating_counts = DiaryStats::get_rating_stats(opt, current_date.daysInMonth());

    render_pie_chart(rating_counts);
    render_polar_chart(opt);
    render_spline_chart(opt);
    render_comparison(rating_counts);

    qDebug() << "Rendered stats for:" << date;
}
