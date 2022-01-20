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

#include <QtNetwork>
#include <utility>

#include "../core/diaryholder.h"
#include "../core/googlewrapper.h"
#include "../core/internalmanager.h"
#include "../util/custommessageboxes.h"
#include "../util/encryptor.h"
#include "../util/eventfilters.h"
#include "../util/misc.h"
#include "asyncfuture.h"
#include "diarymenu.h"
#include "mainmenu.h"
#include "mainwindow.h"
#include "standaloneoptions.h"
#include "ui_mainwindow.h"

MainWindow *main_window_ptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    main_window_ptr = this;

    setWindowTitle(QApplication::applicationName());
    restore_state();
    show_main_menu(false);

    connect(InternalManager::instance()->inactive_filter, &InactiveFilter::sig_inactive_timeout, this,
        &MainWindow::lock_diary);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::instance()
{
    return main_window_ptr;
}

void MainWindow::store_state()
{
    InternalManager::instance()->settings->setValue("geometry", saveGeometry());
    InternalManager::instance()->settings->setValue("window_state", saveState());
    qDebug() << "Stored window state.";
}

void MainWindow::restore_state()
{
    auto geo = InternalManager::instance()->settings->value("geometry", "").toByteArray();
    auto state = InternalManager::instance()->settings->value("window_state", "").toByteArray();
    restoreGeometry(geo);
    restoreState(state);
    qDebug() << "Restored window state.";
}

// The locked parameter indicates if the diary was locked due to inactivity. If it was, then, don't display any dialogs.
void MainWindow::exit_diary_to_main_menu(bool const locked)
{
    static QString primary_id;
    static QString secondary_id;
    auto intman = InternalManager::instance();
    auto gwrapper = GoogleWrapper::instance();
    auto encryptor = Encryptor::instance();

    // Clean up.
    intman->internal_diary_changed = false;
    DiaryHolder::instance()->init();

    // Backup on Google Drive.
    if (intman->settings->value("sync_enabled").toBool() && intman->diary_file_changed) {
        auto check_error = [this, locked](td::NR const &reply) {
            if (QNetworkReply::NoError != reply.error) {
                if (!locked) {
                    AppBusyLock lock;
                    cmb::display_google_drive_network_error(this, std::move(reply.error_message));
                }

                return false;
            }

            return true;
        };

        auto cb_final = [this, check_error, gwrapper, intman, encryptor, locked](td::NR const &reply) {
            AppBusyLock lock;

            if (!check_error(reply))
                return;

            intman->diary_file_changed = false;
            gwrapper->encrypt_credentials();
            gwrapper->google->unlink();
            encryptor->reset();

            if (!locked)
                cmb::diary_uploaded(this);
        };

        auto upload_subroutine = [this, gwrapper, intman, locked, cb_final]() {
            auto file_ptr = new QFile(QString("%1/diary.dat").arg(intman->data_location()));

            if (!file_ptr->open(QIODevice::ReadOnly)) {
                AppBusyLock lock;

                if (!locked)
                    cmb::display_local_diary_access_error(this);

                return;
            }

            if (primary_id.isEmpty())
                gwrapper->upload_file(file_ptr, "diary.dat").subscribe(cb_final);
            else
                gwrapper->update_file(file_ptr, primary_id).subscribe(cb_final);
        };

        auto cb4 = [this, check_error, upload_subroutine, gwrapper, cb_final](td::NR const &reply) {
            if (!check_error(reply))
                return;

            upload_subroutine();
        };

        auto cb3 = [this, check_error, upload_subroutine, gwrapper, cb4, cb_final](td::NR const &reply) {
            if (!check_error(reply))
                return;

            if (secondary_id.isEmpty())
                return upload_subroutine();

            gwrapper->delete_file(secondary_id).subscribe(cb4);
        };

        auto cb2 = [this, check_error, upload_subroutine, gwrapper, cb3, cb_final](td::NR const &reply) {
            if (!check_error(reply))
                return;

            auto const &[id1, id2] = gwrapper->get_file_ids(reply.response);
            primary_id = id1;
            secondary_id = id2;

            if (id1.isEmpty())
                return upload_subroutine();

            gwrapper->copy_file(id1, "diary.dat.bak").subscribe(cb3);
        };

        auto cb1 = [this, gwrapper, locked, cb2]() {
            AppBusyLock lock;

            switch (gwrapper->verify_auth()) {
            case td::LinkingResponse::ScopeMismatch:
                if (!locked)
                    cmb::display_google_drive_scope_mismatch(this);
                break;
            case td::LinkingResponse::Fail:
                if (!locked)
                    cmb::display_google_drive_auth_error(this);
                break;
            case td::LinkingResponse::OK:
                lock.persist = true;
                gwrapper->list_files().subscribe(cb2);
            }
        };

        AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
        AppBusyLock lock(true);
        gwrapper->google->link();
    }
    else {
        gwrapper->encrypt_credentials();
        gwrapper->google->unlink();
        encryptor->reset();
    }

    qDebug() << "Calling show_main_menu from exit_diary_to_main_menu.";
    show_main_menu(locked);
}

void MainWindow::lock_diary()
{
    qDebug() << "Timeout detected.";
    if (td::Window::Main == current_window || td::Window::Options == current_window) {
        // This isn't implemented because I don't believe it's a high risk for those tokens to be leaked?
        // In any case, if that ever DOES prove a threat, I can just uncomment the code below.
        // GoogleWrapper::instance()->google->unlink();
        // qDebug() << "Unlinked potential Google due to inactivity on the main menu.";
        return;
    }

    emit sig_update_diary(true);

    qDebug() << "Locking diary.";
    exit_diary_to_main_menu(true);
}

void MainWindow::clear_grid()
{
    misc::clear_message_boxes();

    QLayoutItem *child;
    while ((child = ui->central_grid->takeAt(0))) {
        qDebug() << "Clearing grid of:" << child->widget() << child;
        child->widget()->deleteLater();
        delete child;
    }
}

void MainWindow::show_main_menu(bool const show_locked_message)
{
    current_window = td::Window::Main;

    clear_grid();
    ui->central_grid->addWidget(new MainMenu(show_locked_message, this));
}

void MainWindow::show_options_menu()
{
    current_window = td::Window::Options;

    clear_grid();
    ui->central_grid->addWidget(new StandaloneOptions(this));
}

void MainWindow::show_diary_menu()
{
    current_window = td::Window::Editor;

    clear_grid();
    ui->central_grid->addWidget(new DiaryMenu(this));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (InternalManager::instance()->app_busy) {
        event->ignore();

        AppBusyLock lock;

        auto cb = [](int const res) {
            if (QMessageBox::Yes == res) {
                qDebug() << "App forced closed by the user.";
                QCoreApplication::exit(1);
            }
            else {
                AppBusyLock lock(true);
                qDebug() << "User rejected force close dialog.";
            }
        };

        auto msgbox = new QMessageBox(this);
        msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
        msgbox->setText("The application is currently performing an action. If you choose to close it now, all unsaved "
                        "changes will be lost!");
        msgbox->setInformativeText("Are you sure you want to force close the application?");
        msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgbox->setDefaultButton(QMessageBox::No);
        QObject::connect(msgbox, &QMessageBox::finished, cb);
        msgbox->show();
    }
    else if (td::Window::Options == current_window) {
        event->ignore(); // Don't close the main window, but exit to the main menu.
        show_main_menu(false);
    }
    else if (td::Window::Editor == current_window) {
        event->ignore(); // Don't close the main window, but exit to the main menu.

        if (InternalManager::instance()->internal_diary_changed) {
            auto cb = [this](int const res) {
                if (QMessageBox::Cancel == res)
                    return;

                if (QMessageBox::Save == res) {
                    AppBusyLock lock;

                    // If the diary failed to save, don't exit to the main menu.
                    if (!DiaryHolder::instance()->save())
                        cmb::display_local_diary_save_error(this);
                    else
                        exit_diary_to_main_menu(false);

                    return;
                }

                exit_diary_to_main_menu(false);
            };

            auto msgbox = new QMessageBox(this);
            msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
            msgbox->setText("There are unsaved changes!");
            msgbox->setInformativeText("Would you like to save them?");
            msgbox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgbox->setDefaultButton(QMessageBox::Save);
            QObject::connect(msgbox, &QMessageBox::finished, cb);
            msgbox->show();
        }
        else {
            exit_diary_to_main_menu(false);
        }
    }
    else {
        store_state();
        qDebug() << "Exiting application.";
        event->accept(); // Actually exit the application.
    }
}
