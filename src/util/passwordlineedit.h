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

// The code in this file was adapted from https://stackoverflow.com/a/51194796

#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QtWidgets>

class PasswordLineEdit : public QLineEdit {
public:
    PasswordLineEdit(QWidget *parent = nullptr);

private slots:
    void on_pressed();
    void on_released();

private:
    QToolButton *button;

    QString get_eye_icon(bool const on);
};

#endif // PASSWORDLINEEDIT_H
