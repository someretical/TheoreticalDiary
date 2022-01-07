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

#ifndef DIARYPIXELS_H
#define DIARYPIXELS_H

#include "../core/diaryholder.h"
#include "../core/internalmanager.h"
#include "../util/misc.h"

#include <QtWidgets>
#include <algorithm>
#include <memory>

namespace Ui {
class DiaryPixels;
}

class DiaryPixels : public QWidget {
    Q_OBJECT

signals:
    void sig_changed_size(int const new_width);

public:
    explicit DiaryPixels(QWidget *parent = nullptr);
    ~DiaryPixels();

    void resizeEvent(QResizeEvent *);
    int calculate_size();

    QDate current_date;
    std::vector<std::unique_ptr<QString>> rating_stylesheets;
    QString white_star;
    QString black_star;

public slots:
    void update_theme();
    void render_grid();
    void export_image();

private:
    Ui::DiaryPixels *ui;
};

class PixelLabel : public QLabel {
    Q_OBJECT

public:
    explicit PixelLabel(
        td::Rating const r, bool const special, QDate const &date, int const size, QWidget *parent = nullptr);
    ~PixelLabel();

public slots:
    void resize(int const new_width);
};

#endif // DIARYPIXELS_H
