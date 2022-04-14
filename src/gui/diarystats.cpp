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
#include "../util/misc.h"
#include "ui_diarystats.h"

using namespace QtCharts;

char const *ratings[] = {"Unknown", "Very bad", "Bad", "OK", "Good", "Very good"};

qreal const angular_min = 1;
// qreal const angular_max = 31;
qreal const radial_min = static_cast<int>(td::Rating::Unknown);
qreal const radial_max = static_cast<int>(td::Rating::VeryGood);

auto const dark_background = QColor(49, 54, 59, 170);
auto const light_background = QColor(230, 230, 230, 170);
auto const dark_white = light_background.darker();
auto const light_black = dark_background.lighter();

DiaryStats::DiaryStats(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryStats)
{
    m_ui->setupUi(this);

    // Give DiaryComparisonLabels proper attributes.
    m_ui->very_bad->m_rating = td::Rating::VeryBad;
    m_ui->bad->m_rating = td::Rating::Bad;
    m_ui->ok->m_rating = td::Rating::Ok;
    m_ui->good->m_rating = td::Rating::Good;
    m_ui->very_good->m_rating = td::Rating::VeryGood;
    m_ui->unknown->m_rating = td::Rating::Unknown;
    m_ui->starred->m_special = true;

    m_ui->very_bad_2->m_rating = td::Rating::VeryBad;
    m_ui->bad_2->m_rating = td::Rating::Bad;
    m_ui->ok_2->m_rating = td::Rating::Ok;
    m_ui->good_2->m_rating = td::Rating::Good;
    m_ui->very_good_2->m_rating = td::Rating::VeryGood;
    m_ui->unknown_2->m_rating = td::Rating::Unknown;
    m_ui->starred_2->m_special = true;

    m_ui->very_bad_3->m_rating = td::Rating::VeryBad;
    m_ui->bad_3->m_rating = td::Rating::Bad;
    m_ui->ok_3->m_rating = td::Rating::Ok;
    m_ui->good_3->m_rating = td::Rating::Good;
    m_ui->very_good_3->m_rating = td::Rating::VeryGood;
    m_ui->unknown_3->m_rating = td::Rating::Unknown;
    m_ui->starred_3->m_special = true;

    m_ui->very_bad->update_tooltip();
    m_ui->bad->update_tooltip();
    m_ui->ok->update_tooltip();
    m_ui->good->update_tooltip();
    m_ui->very_good->update_tooltip();
    m_ui->unknown->update_tooltip();
    m_ui->starred->update_tooltip();

    m_ui->very_bad_2->update_tooltip();
    m_ui->bad_2->update_tooltip();
    m_ui->ok_2->update_tooltip();
    m_ui->good_2->update_tooltip();
    m_ui->very_good_2->update_tooltip();
    m_ui->unknown_2->update_tooltip();
    m_ui->starred_2->update_tooltip();

    m_ui->very_bad_3->update_tooltip();
    m_ui->bad_3->update_tooltip();
    m_ui->ok_3->update_tooltip();
    m_ui->good_3->update_tooltip();
    m_ui->very_good_3->update_tooltip();
    m_ui->unknown_3->update_tooltip();
    m_ui->starred_3->update_tooltip();

    // Make the numbers display in a monospaced font.
    auto monospaced = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monospaced.setLetterSpacing(QFont::PercentageSpacing, 110);

    m_ui->l0->setFont(monospaced);
    m_ui->l1->setFont(monospaced);
    m_ui->l2->setFont(monospaced);
    m_ui->l3->setFont(monospaced);
    m_ui->l4->setFont(monospaced);
    m_ui->l5->setFont(monospaced);
    m_ui->ls->setFont(monospaced);

    m_ui->t0->setFont(monospaced);
    m_ui->t1->setFont(monospaced);
    m_ui->t2->setFont(monospaced);
    m_ui->t3->setFont(monospaced);
    m_ui->t4->setFont(monospaced);
    m_ui->t5->setFont(monospaced);
    m_ui->ts->setFont(monospaced);

    m_ui->d0->setFont(monospaced);
    m_ui->d1->setFont(monospaced);
    m_ui->d2->setFont(monospaced);
    m_ui->d3->setFont(monospaced);
    m_ui->d4->setFont(monospaced);
    m_ui->d5->setFont(monospaced);
    m_ui->ds->setFont(monospaced);

    // Initilise pie chart.
    auto pie_chart = new QChart();
    pie_chart->setMargins(QMargins(0, 0, 0, 0));
    pie_chart->setBackgroundVisible(false);
    pie_chart->legend()->hide();
    pie_chart->setEnabled(false); // Disables scrolling (at the cost of what?).
    m_ui->pie_chart_view->setRenderHint(QPainter::Antialiasing);
    m_ui->pie_chart_view->setChart(pie_chart);

    // Initialise polar chart.
    auto polar_chart = new QPolarChart();
    polar_chart->setMargins(QMargins(0, 0, 0, 0));
    polar_chart->setBackgroundVisible(false);
    polar_chart->legend()->hide();
    polar_chart->setEnabled(false);
    m_ui->polar_chart_view->setRenderHint(QPainter::Antialiasing);
    m_ui->polar_chart_view->setChart(polar_chart);

    // Initialise spline chart.
    auto spline_chart = new QChart();
    spline_chart->setMargins(QMargins(0, 0, 0, 0));
    spline_chart->setBackgroundVisible(false);
    spline_chart->legend()->hide();
    spline_chart->setEnabled(false);
    m_ui->spline_chart_view->setRenderHint(QPainter::Antialiasing);
    m_ui->spline_chart_view->setChart(spline_chart);

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryStats::render_stats,
        Qt::QueuedConnection);

    // Navigator slots.
    connect(m_ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DiaryStats::month_changed,
        Qt::QueuedConnection);
    connect(m_ui->year_edit, &QDateEdit::dateChanged, this, &DiaryStats::year_changed, Qt::QueuedConnection);
    connect(m_ui->next_month, &QPushButton::clicked, this, &DiaryStats::next_month, Qt::QueuedConnection);
    connect(m_ui->prev_month, &QPushButton::clicked, this, &DiaryStats::prev_month, Qt::QueuedConnection);

    // Next/prev month keyboard shortcuts
    m_next_month_bind = new QShortcut(Qt::Key_Down, this);
    m_next_month_bind->setAutoRepeat(true);
    connect(m_next_month_bind, &QShortcut::activated, this, &DiaryStats::next_month, Qt::QueuedConnection);
    m_prev_month_bind = new QShortcut(Qt::Key_Up, this);
    m_prev_month_bind->setAutoRepeat(true);
    connect(m_prev_month_bind, &QShortcut::activated, this, &DiaryStats::prev_month, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryStats::update_theme,
        Qt::QueuedConnection);

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryStats::~DiaryStats()
{
    delete m_ui;
    delete m_next_month_bind;
    delete m_prev_month_bind;
}

void DiaryStats::update_theme()
{
    render_stats(QDate::currentDate());
}

void DiaryStats::next_month()
{
    auto const &&next = m_ui->year_edit->date().addMonths(1);
    if (next.isValid())
        render_stats(next);
}

void DiaryStats::prev_month()
{
    auto const &&prev = m_ui->year_edit->date().addMonths(-1);
    if (prev.isValid())
        render_stats(prev);
}

void DiaryStats::month_changed(int const)
{
    render_stats(QDate(m_ui->year_edit->date().year(), m_ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryStats::year_changed(QDate const &date)
{
    if (date.isValid())
        render_stats(QDate(m_ui->year_edit->date().year(), m_ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryStats::render_pie_chart(std::vector<int> const &rating_counts)
{
    auto theme = InternalManager::instance()->get_theme();
    // Clear all data from the chart so new values can be added.
    m_ui->pie_chart_view->chart()->removeAllSeries();

    auto *pie_series = new QPieSeries();
    pie_series->setPieEndAngle(330);

    if (m_current_date.daysInMonth() == rating_counts[0]) {
        // If there is no data for the current month, display an empty pie chart.
        pie_series->append(ratings[0], m_current_date.daysInMonth());
        auto slice = pie_series->slices().at(0);
        slice->setBrush(InternalManager::instance()->colour(td::ColourRole::Unknown));
        slice->setLabelColor(InternalManager::instance()->colour(td::ColourRole::Text));
        slice->setLabelVisible();
        slice->setBorderWidth(4);
        slice->setBorderColor(td::Theme::Dark == theme ? light_black : dark_white);
    }
    else {
        // Display pie chart slices from largest to smallest.
        auto sorted = std::vector<std::pair<td::Rating, int>>();

        switch (static_cast<td::settings::PieSliceSort>(
            InternalManager::instance()->m_settings->value("pie_slice_sort").toInt())) {
        case td::settings::PieSliceSort::Category:
            for (int i = 1; i < 6; ++i)
                if (0 != rating_counts[i])
                    sorted.push_back(std::pair(static_cast<td::Rating>(i), rating_counts[i]));

            break;
        case td::settings::PieSliceSort::Days:
            for (int i = 1; i < 6; ++i)
                if (0 != rating_counts[i])
                    sorted.push_back(std::pair(static_cast<td::Rating>(i), rating_counts[i]));

            std::sort(sorted.begin(), sorted.end(), [](auto const &a, auto const &b) { return a.second > b.second; });
            break;
        }

        for (auto const &[rating, count] : sorted) {
            pie_series->append(
                QString("%1 - %2 day%3")
                    .arg(QString(ratings[static_cast<int>(rating)]), QString::number(count), 1 == count ? "" : "s"),
                count);

            auto slice = pie_series->slices().at(pie_series->slices().size() - 1);
            slice->setBorderWidth(4);
            slice->setBorderColor(td::Theme::Dark == theme ? light_black : dark_white);
            slice->setLabelColor(InternalManager::instance()->colour(td::ColourRole::Text));
            slice->setLabelVisible();
            slice->setBrush(misc::rating_to_colour(rating));
        }
    }

    m_ui->pie_chart_view->chart()->addSeries(pie_series);
    m_ui->pie_chart_view->update();
    qDebug() << "Rendered pie chart.";
}

void DiaryStats::render_polar_chart(std::optional<td::YearMap::iterator> const &opt)
{
    auto theme = InternalManager::instance()->get_theme();
    auto chart = qobject_cast<QPolarChart *>(m_ui->polar_chart_view->chart());
    auto const angular_max = m_current_date.daysInMonth();

    chart->removeAllSeries();
    for (auto const &i : chart->axes())
        chart->removeAxis(i);

    auto angular_axis = new QValueAxis();
    angular_axis->setTickCount(angular_max + 1);
    angular_axis->setLabelFormat("%d");
    angular_axis->setLabelsColor(InternalManager::instance()->colour(td::ColourRole::Text));
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
            auto const &[dummy, rating, d2, d3, d4, d5] = data;

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
                area->setColor(misc::rating_to_colour(rating));
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
    auto chart = m_ui->spline_chart_view->chart();

    chart->removeAllSeries();
    for (auto const &i : chart->axes())
        chart->removeAxis(i);

    auto x_axis = new QValueAxis();
    chart->addAxis(x_axis, Qt::AlignBottom);

    switch (m_current_date.daysInMonth()) {
    case 28:
        x_axis->setRange(0, m_current_date.daysInMonth());
        x_axis->setTickAnchor(0);
        x_axis->setTickInterval(2);
        x_axis->setTickCount(15);
        break;
    case 29:
        x_axis->setRange(1, m_current_date.daysInMonth());
        x_axis->setTickAnchor(1);
        x_axis->setTickInterval(2);
        x_axis->setTickCount(15);
        break;
    case 30:
        x_axis->setRange(0, m_current_date.daysInMonth());
        x_axis->setTickAnchor(0);
        x_axis->setTickInterval(2);
        x_axis->setTickCount(16);
        break;
    case 31:
        x_axis->setRange(1, m_current_date.daysInMonth());
        x_axis->setTickAnchor(1);
        x_axis->setTickInterval(2);
        x_axis->setTickCount(16);
        break;
    default:
        // This will probably happen if somebody puts the year all the way back to 1700s when the calendar was reset or
        // something really obscure like that.
        x_axis->setRange(1, m_current_date.daysInMonth());
        x_axis->setTickAnchor(1);
        x_axis->setTickInterval(2);
        x_axis->setTickCount(15);
        qDebug() << "This month has an invalid number of days in it:" << m_current_date;
        break;
    }

    x_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    x_axis->setLabelFormat("%d");
    x_axis->setLabelsColor(InternalManager::instance()->colour(td::ColourRole::Text));
    x_axis->setLabelsVisible();
    x_axis->setTitleText("Day");
    x_axis->setTitleBrush(QBrush(InternalManager::instance()->colour(td::ColourRole::Text)));

    auto y_axis = new QValueAxis();
    chart->addAxis(y_axis, Qt::AlignLeft);
    y_axis->setRange(1, 5);
    y_axis->setTickCount(5);
    y_axis->setGridLineColor(td::Theme::Dark == theme ? light_black : dark_white);
    y_axis->setLabelFormat("%d");
    y_axis->setLabelsColor(InternalManager::instance()->colour(td::ColourRole::Text));
    y_axis->setLabelsVisible();
    y_axis->setTitleText("Rating");
    y_axis->setTitleBrush(QBrush(InternalManager::instance()->colour(td::ColourRole::Text)));

    auto scatter_series = new QScatterSeries();
    chart->addSeries(scatter_series);
    scatter_series->attachAxis(x_axis);
    scatter_series->attachAxis(y_axis);
    scatter_series->setMarkerSize(8);
    scatter_series->setColor(InternalManager::instance()->colour(td::ColourRole::Text));
    scatter_series->setBorderColor(QColor(Qt::transparent));

    if (opt) {
        auto prev_day = 0;
        auto current_spline_series = new QSplineSeries();
        chart->addSeries(current_spline_series);
        current_spline_series->attachAxis(x_axis);
        current_spline_series->attachAxis(y_axis);
        current_spline_series->setColor(InternalManager::instance()->colour(td::ColourRole::Text));

        for (auto const &[day, data] : (*opt)->second) {
            auto const &[important, rating, dummy, d2, d3, d4] = data;
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
                    current_spline_series->setColor(InternalManager::instance()->colour(td::ColourRole::Text));
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
    m_ui->t0->setText(QString::number(rating_counts[0]));
    m_ui->t1->setText(QString::number(rating_counts[1]));
    m_ui->t2->setText(QString::number(rating_counts[2]));
    m_ui->t3->setText(QString::number(rating_counts[3]));
    m_ui->t4->setText(QString::number(rating_counts[4]));
    m_ui->t5->setText(QString::number(rating_counts[5]));
    m_ui->ts->setText(QString::number(rating_counts[6]));

    auto const &prev_month = m_current_date.addMonths(-1);
    if (!prev_month.isValid()) {
        m_ui->l0->setText("N/A");
        m_ui->l1->setText("N/A");
        m_ui->l2->setText("N/A");
        m_ui->l3->setText("N/A");
        m_ui->l4->setText("N/A");
        m_ui->l5->setText("N/A");
        m_ui->ls->setText("N/A");

        m_ui->d0->setText("N/A");
        m_ui->d1->setText("N/A");
        m_ui->d2->setText("N/A");
        m_ui->d3->setText("N/A");
        m_ui->d4->setText("N/A");
        m_ui->d5->setText("N/A");
        m_ui->ds->setText("N/A");
        return;
    }

    auto const &prev_stats =
        DiaryStats::get_rating_stats(DiaryHolder::instance()->get_monthmap(prev_month), prev_month.daysInMonth());

    m_ui->l0->setText(QString::number(prev_stats[0]));
    m_ui->l1->setText(QString::number(prev_stats[1]));
    m_ui->l2->setText(QString::number(prev_stats[2]));
    m_ui->l3->setText(QString::number(prev_stats[3]));
    m_ui->l4->setText(QString::number(prev_stats[4]));
    m_ui->l5->setText(QString::number(prev_stats[5]));
    m_ui->ls->setText(QString::number(prev_stats[6]));

    m_ui->d0->setText(QString::number(rating_counts[0] - prev_stats[0]));
    m_ui->d1->setText(QString::number(rating_counts[1] - prev_stats[1]));
    m_ui->d2->setText(QString::number(rating_counts[2] - prev_stats[2]));
    m_ui->d3->setText(QString::number(rating_counts[3] - prev_stats[3]));
    m_ui->d4->setText(QString::number(rating_counts[4] - prev_stats[4]));
    m_ui->d5->setText(QString::number(rating_counts[5] - prev_stats[5]));
    m_ui->ds->setText(QString::number(rating_counts[6] - prev_stats[6]));

    qDebug() << "Rendered stats table.";
}

std::vector<int> DiaryStats::get_rating_stats(std::optional<td::YearMap::iterator> const &opt, int const total_days)
{
    auto rating_counts = std::vector<int>{total_days, 0, 0, 0, 0, 0, 0};

    if (opt) {
        for (auto const &[day, data] : (*opt)->second) {
            auto const &[important, rating, dummy, d2, d3, d4] = data;
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
    m_current_date = date;

    // Update the selector UI.
    const QSignalBlocker b1(m_ui->month_dropdown);
    const QSignalBlocker b2(m_ui->year_edit);

    if (date.isValid()) {
        m_ui->month_dropdown->setCurrentIndex(date.month() - 1);
        m_ui->year_edit->setDate(date);
    }
    else {
        m_ui->month_dropdown->setCurrentIndex(m_current_date.month() - 1);
        m_ui->year_edit->setDate(m_current_date);
    }

    auto const &opt = DiaryHolder::instance()->get_monthmap(m_current_date);
    auto const &rating_counts = DiaryStats::get_rating_stats(opt, m_current_date.daysInMonth());

    render_pie_chart(rating_counts);
    render_polar_chart(opt);
    render_spline_chart(opt);
    render_comparison(rating_counts);

    qDebug() << "Rendered stats for:" << date;
}
