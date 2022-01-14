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

#ifndef DIARYSTATS_H
#define DIARYSTATS_H

#include <QtWidgets>
#include <optional>
#include <vector>

#include "../core/internalmanager.h"

namespace Ui {
class DiaryStats;
}

class DiaryStats : public QWidget {
    Q_OBJECT

public:
    explicit DiaryStats(QWidget *parent = nullptr);
    ~DiaryStats();

    static std::vector<int> get_rating_stats(std::optional<td::YearMap::iterator> const &opt, int const total_days);

    void render_pie_chart(std::vector<int> const &rating_counts);
    void render_polar_chart(std::optional<td::YearMap::iterator> const &opt);
    void render_spline_chart(std::optional<td::YearMap::iterator> const &opt);
    void render_comparison(std::vector<int> const &rating_counts);

    QDate current_date;

public slots:
    void update_theme();
    void render_stats(QDate const &date);
    void next_month();
    void prev_month();
    void month_changed(int const month);
    void year_changed(QDate const &date);

private:
    Ui::DiaryStats *ui;
};

#endif // DIARYSTATS_H
