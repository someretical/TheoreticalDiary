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

#ifndef DIARYMENU_H
#define DIARYMENU_H

#include <QtWidgets>

namespace Ui {
class DiaryMenu;
}

class DiaryMenu : public QWidget {
    Q_OBJECT

public:
    explicit DiaryMenu(QWidget *parent = nullptr);
    ~DiaryMenu();

public slots:
    void tab_changed(int const);

private:
    Ui::DiaryMenu *ui;
};

#endif // DIARYMENU_H
