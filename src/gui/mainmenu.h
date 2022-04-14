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

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QtWidgets>

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget {
    Q_OBJECT

public:
    explicit MainMenu(bool const show_locked_message, QWidget *parent = nullptr);
    ~MainMenu();

public slots:
    void decrypt_diary();
    void new_diary(bool const skip_overwrite_check = false);
    void import_diary();
    void open_options();
    bool get_diary_contents();

private:
    Ui::MainMenu *m_ui;
    QShortcut *m_enter_bind;
};

#endif // MAINMENU_H
