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

#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include <QtWidgets>

namespace Ui {
class OptionsMenu;
}

class OptionsMenu : public QWidget {
    Q_OBJECT

public:
    explicit OptionsMenu(bool const from_diary_editor, QWidget *parent = nullptr);
    ~OptionsMenu();

public slots:
    void back();
    void save_settings();
    void setup_layout();
    void export_diary();
    void update_lock_timeout();
    void change_password();
    void refresh_linked_checkbox();
    void complete_oauth();
    void download_backup();
    void upload_diary();
    void flush_oauth();
    void dev_list();
    void dev_upload();
    void dev_download();
    void dev_update();
    void dev_copy();
    void dev_delete();
    void show_about();
    void reset_settings();
    void test();

private:
    Ui::OptionsMenu *ui;
    bool diary_editor_mode;
};

#endif // OPTIONSMENU_H
