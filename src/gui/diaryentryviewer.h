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

class DiaryEntryViewer;
namespace td {
struct LabelData {
    DiaryEntryViewer *parent;
    int day;
    td::Rating rating;
    bool special;
};
} // namespace td

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
};

class DiaryEntryDayLabel : public QLabel {
    Q_OBJECT

public:
    explicit DiaryEntryDayLabel(td::LabelData const &d, QWidget *parent = nullptr);
    ~DiaryEntryDayLabel();

    td::LabelData data;

public slots:
    void update_theme();
};

class DiaryEntryDayMessage : public QLabel {
    Q_OBJECT

public:
    explicit DiaryEntryDayMessage(std::string const &m, QWidget *parent = nullptr);
    ~DiaryEntryDayMessage();

    std::string message;
    bool expanded;

public slots:
    void update_theme();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // DIARYENTRYVIEWER_H
