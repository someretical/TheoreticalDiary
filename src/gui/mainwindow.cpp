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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow *main_window_ptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QApplication::applicationName());

    main_window_ptr = this;
    current_window = td::Window::Main;
    last_window = td::Window::Main;

    previous_state = Qt::ApplicationActive;
    timer = new QTimer(this);
    timer->setTimerType(Qt::CoarseTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::inactive_time_up, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &MainWindow::update_theme,
        Qt::QueuedConnection);
    update_theme();

    show_main_menu();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

MainWindow *MainWindow::instance()
{
    return main_window_ptr;
}

void MainWindow::focus_changed(Qt::ApplicationState const state)
{
    if (Qt::ApplicationActive == state) {
        if (td::Window::Editor == current_window || td::Window::Editor == last_window)
            timer->stop();
    }
    else if (Qt::ApplicationInactive == state) {
        if ((td::Window::Editor == last_window && td::Window::Options == current_window) ||
            td::Window::Editor == current_window)
            timer->start(300000); // 5 mins = 300000 ms
    }

    previous_state = state;
}

void MainWindow::exit_diary_to_main_menu()
{
    // Clean up.
    InternalManager::instance()->internal_diary_changed = false;
    DiaryHolder::instance()->init();

    // Backup on Google Drive.
    if (InternalManager::instance()->settings->value("sync_enabled", false).toBool() &&
        InternalManager::instance()->diary_file_changed) {
        GoogleWrapper::instance()->upload_diary([this](const td::GWrapperError err) {
            InternalManager::instance()->diary_file_changed = false;
            GoogleWrapper::instance()->encrypt_credentials();
            Encryptor::instance()->reset();
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

            switch (err) {
            case td::GWrapperError::Auth:
                cmb::display_auth_error(this);
                break;
            case td::GWrapperError::Network:
                cmb::display_network_error(this);
                break;
            case td::GWrapperError::IO:
                cmb::display_io_error(this);
                break;
            case td::GWrapperError::DriveFile:
                cmb::display_drive_file_error(this);
                break;
            case td::GWrapperError::None:
                cmb::diary_uploaded(this);
                break;
            }
        });
    }
    else {
        Encryptor::instance()->reset();
    }

    show_main_menu();
}

void MainWindow::inactive_time_up()
{
    timer->stop();

    if (td::Window::Main == current_window ||
        (td::Window::Options == current_window && td::Window::Main == last_window))
        return;

    emit sig_update_diary();
    if (InternalManager::instance()->internal_diary_changed)
        DiaryHolder::instance()->save(); // Ignore any errors.

    exit_diary_to_main_menu();
}

void MainWindow::update_theme()
{
    QFile file(QString(":/%1/material_cyan_dark.qss").arg(InternalManager::instance()->get_theme()));
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}

void MainWindow::clear_grid()
{
    // This code is so unbelievably janky, there is a ONE HUNDRED PERCENT chance it will bite me back in the future.
    // Basically, if there is some modal widget like a dialog or message box that is open and is assigned on the stack
    // when clear_grid() is called, the application will seg fault because Qt tries to free memory allocated on the
    // stack. The fix for this is to close all active modal widgets before deleting the parent widget. NOTE THAT CLOSE
    // != DELETE. IF THE MODAL WAS DYNAMICALLY CREATED, THE WA_DELETE_ON_CLOSE FLAG NEEDS TO BE SET. I don't know if
    // this will cause another seg fault though. Also for some reason, the compiler wants me to enclose the assignment
    // in the while loop in another set of parentheses. :o
    QWidget *w;
    while ((w = QApplication::activeModalWidget()))
        w->close();

    QLayoutItem *child;
    while ((child = ui->central_widget->layout()->takeAt(0))) {
        child->widget()->deleteLater();
        delete child;
    }
}

void MainWindow::show_main_menu()
{
    last_window = current_window;
    current_window = td::Window::Main;

    clear_grid();
    ui->central_widget->layout()->addWidget(new MainMenu(this));
}

void MainWindow::show_options_menu()
{
    auto const on_diary_editor = td::Window::Editor == current_window;
    last_window = current_window;
    current_window = td::Window::Options;

    clear_grid();
    ui->central_widget->layout()->addWidget(new OptionsMenu(on_diary_editor, this));
}

void MainWindow::show_diary_menu()
{
    last_window = current_window;
    current_window = td::Window::Editor;

    clear_grid();
    ui->central_widget->layout()->addWidget(new DiaryMenu(this));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (InternalManager::instance()->app_busy) {
        event->ignore();
    }
    else if (td::Window::Options == current_window && td::Window::Editor == last_window) {
        event->ignore(); // Don't close the main window, but exit to the diary menu.
        show_diary_menu();
    }
    else if (td::Window::Options == current_window && td::Window::Main == last_window) {
        event->ignore(); // Don't close the main window, but exit to the main menu.
        show_main_menu();
    }
    else if (td::Window::Editor == current_window) {
        event->ignore(); // Don't close the main window, but exit to the main menu.

        if (InternalManager::instance()->internal_diary_changed) {
            switch (cmb::confirm_exit_to_main_menu(this)) {
            case QMessageBox::AcceptRole: {
                // If the diary failed to save, don't exit to the main menu.
                InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
                auto saved = DiaryHolder::instance()->save();
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

                if (!saved)
                    return cmb::save_error(this);

                break;
            }

            // Cancel button was pressed.
            case QMessageBox::RejectRole:
                return;
            }
        }

        exit_diary_to_main_menu();
    }
    else {
        event->accept(); // Actually exit the application.
    }
}
