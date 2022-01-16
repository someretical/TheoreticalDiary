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

#ifndef DIARYENTRYVIEWER_H
#define DIARYENTRYVIEWER_H

#include <QtWidgets>
#include <memory>

#include "../core/internalmanager.h"

namespace Ui {
class DiaryEntryViewer;
}

class DiaryEntryViewer : public QWidget {
    Q_OBJECT

signals:
    void sig_update_labels();

public:
    explicit DiaryEntryViewer(QWidget *parent = nullptr);
    ~DiaryEntryViewer();

    QDate current_date;
    std::vector<std::unique_ptr<QString>> rating_stylesheets;
    QString white_star;
    QString black_star;

public slots:
    void update_theme();
    void change_month(QDate const &date);
    void next_month();
    void prev_month();
    void month_changed(int const month);
    void year_changed(QDate const &date);

private:
    Ui::DiaryEntryViewer *ui;

    QByteArray generate_base64_icon(int const day, td::Rating const rating, bool const important);
    QPixmap generate_background(td::Rating const rating);
    QPixmap generate_star(td::Rating const rating);
    QPixmap generate_text(int const day_, td::Rating const rating);
};

#endif // DIARYENTRYVIEWER_H
