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

#ifndef DIARYSTATS_H
#define DIARYSTATS_H

#include "diaryeditor.h"

#include <QtWidgets>
#include <optional>

namespace Ui {
class DiaryStats;
}

class DiaryStats : public QWidget {
    Q_OBJECT

public:
    explicit DiaryStats(const DiaryEditor *editor, QWidget *parent = nullptr);
    ~DiaryStats();

    static std::vector<int> get_rating_stats(const std::optional<td::YearMap::iterator> &opt, const int total_days);

    void render_pie_chart(const std::vector<int> &rating_counts);
    void render_polar_chart(const std::optional<td::YearMap::iterator> &opt);
    void render_spline_chart(const std::optional<td::YearMap::iterator> &opt);
    void render_comparison(const std::vector<int> &rating_counts);

    QDate *current_month;

public slots:
    void apply_theme();
    void render_stats(const QDate &date, const bool ignore_month_check);
    void next_month();
    void prev_month();
    void month_changed(const int month);
    void year_changed(const QDate &date);

private:
    Ui::DiaryStats *ui;
};

#endif // DIARYSTATS_H
