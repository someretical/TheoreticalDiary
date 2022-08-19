/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THEORETICAL_DIARY_PASSWORDLINEEDIT_H
#define THEORETICAL_DIARY_PASSWORDLINEEDIT_H

#include <QLineEdit>

class PasswordLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit PasswordLineEdit(QWidget *parent = nullptr);
    ~PasswordLineEdit() override;

private:
    QAction *m_actionMatch;
    QAction *m_actionMismatch;
    QAction *m_actionToggleVisibility;

private slots:
    void toggleVisibility(bool show);
};

#endif // THEORETICAL_DIARY_PASSWORDLINEEDIT_H
