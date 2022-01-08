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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../core/diaryholder.h"
#include "../core/googlewrapper.h"
#include "../core/internalmanager.h"
#include "../util/custommessageboxes.h"
#include "../util/encryptor.h"
#include "diarymenu.h"
#include "mainmenu.h"
#include "optionsmenu.h"

#include <QtWidgets>
#include <fstream>
#include <json.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

signals:
    void sig_update_diary();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow *instance();
    void clear_grid();
    void exit_diary_to_main_menu(bool const locked);
    void store_state();
    void restore_state();
    void closeEvent(QCloseEvent *event);

    td::Window current_window;
    td::Window last_window;

public slots:
    void lock_diary();
    void update_theme();
    void show_main_menu(bool const show_locked_message);
    void show_diary_menu();
    void show_options_menu();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
