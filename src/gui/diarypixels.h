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

#ifndef DIARYPIXELS_H
#define DIARYPIXELS_H

#include <QtWidgets>
#include <memory>

#include "../core/internalmanager.h"

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
    void setup_grid();
    int calculate_size();

    QDate m_current_date;

public slots:
    void render_button_clicked();
    void render_grid(QDate const &new_date);
    void export_image();

private:
    Ui::DiaryPixels *m_ui;
};

#endif // DIARYPIXELS_H
