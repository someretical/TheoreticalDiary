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

#include "optionsmenu.h"
#include "../core/googlewrapper.h"
#include "../core/theoreticaldiary.h"
#include "../util/custommessageboxes.h"
#include "aboutdialog.h"
#include "apiresponse.h"
#include "licensesdialog.h"
#include "mainwindow.h"
#include "ui_optionsmenu.h"

#include <fstream>

OptionsMenu::OptionsMenu(bool const from_diary_editor, QWidget *parent) : QWidget(parent), ui(new Ui::OptionsMenu)
{
    ui->setupUi(this);

    diary_editor_mode = from_diary_editor;

    connect(ui->ok_button, &QPushButton::clicked, this, &OptionsMenu::back, Qt::QueuedConnection);
    connect(ui->apply_button, &QPushButton::clicked, this, &OptionsMenu::save_settings, Qt::QueuedConnection);
    connect(ui->export_button, &QPushButton::clicked, this, &OptionsMenu::export_diary, Qt::QueuedConnection);
    connect(
        ui->change_password_button, &QPushButton::clicked, this, &OptionsMenu::change_password, Qt::QueuedConnection);
    connect(
        ui->download_backup_button, &QPushButton::clicked, this, &OptionsMenu::download_backup, Qt::QueuedConnection);
    connect(ui->upload_backup_button, &QPushButton::clicked, this, &OptionsMenu::upload_diary, Qt::QueuedConnection);
    connect(ui->flush_oauth_button, &QPushButton::clicked, this, &OptionsMenu::flush_oauth, Qt::QueuedConnection);
    connect(ui->dev_list_files_button, &QPushButton::clicked, this, &OptionsMenu::dev_list, Qt::QueuedConnection);
    connect(ui->dev_upload_file_button, &QPushButton::clicked, this, &OptionsMenu::dev_upload, Qt::QueuedConnection);
    connect(
        ui->dev_download_file_button, &QPushButton::clicked, this, &OptionsMenu::dev_download, Qt::QueuedConnection);
    connect(ui->dev_update_file_button, &QPushButton::clicked, this, &OptionsMenu::dev_update, Qt::QueuedConnection);
    connect(ui->dev_copy_file_button, &QPushButton::clicked, this, &OptionsMenu::dev_copy, Qt::QueuedConnection);
    connect(ui->dev_delete_button, &QPushButton::clicked, this, &OptionsMenu::dev_delete, Qt::QueuedConnection);
    connect(ui->about_button, &QPushButton::clicked, this, &OptionsMenu::show_about, Qt::QueuedConnection);
    connect(ui->licenses_button, &QPushButton::clicked, this, &OptionsMenu::show_licenses, Qt::QueuedConnection);

    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_request_end, this, &OptionsMenu::request_end,
        Qt::QueuedConnection);

    connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this, &OptionsMenu::apply_theme,
        Qt::QueuedConnection);
    apply_theme();

    setup_layout();
}

OptionsMenu::~OptionsMenu()
{
    delete ui;
}

void OptionsMenu::apply_theme()
{
    auto const &theme = TheoreticalDiary::instance()->theme();

    QFile file(QString(":/%1/optionsmenu.qss").arg(theme));
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();
}

void OptionsMenu::back()
{
    if (diary_editor_mode)
        return qobject_cast<MainWindow *>(parentWidget()->parentWidget())->show_diary_menu(QDate::currentDate());

    qobject_cast<MainWindow *>(parentWidget()->parentWidget())->show_main_menu();
}

void OptionsMenu::save_settings()
{
    TheoreticalDiary::instance()->settings->setValue("sync_enabled", ui->sync_checkbox->isChecked());

    if (diary_editor_mode)
        qobject_cast<MainWindow *>(parentWidget()->parentWidget())->save_diary(false);

    back();
}

void OptionsMenu::setup_layout()
{
    ui->alert_text->setText("");

    if (!diary_editor_mode) {
        ui->export_button->setEnabled(false);
        ui->change_password_button->setEnabled(false);
        ui->new_password->setEnabled(false);
        ui->new_password_confirm->setEnabled(false);
        ui->upload_backup_button->setEnabled(false);
        ui->dev_list_files_button->setEnabled(false);
        ui->dev_upload_file_button->setEnabled(false);
        ui->dev_download_file_button->setEnabled(false);
        ui->dev_download_file_id->setEnabled(false);
        ui->dev_update_file_button->setEnabled(false);
        ui->dev_update_file_id->setEnabled(false);
        ui->dev_copy_file_button->setEnabled(false);
        ui->dev_copy_file_id->setEnabled(false);
        ui->dev_copy_file_new_name->setEnabled(false);
        ui->dev_delete_button->setEnabled(false);
        ui->dev_delete_file_id->setEnabled(false);
    }

    ui->sync_checkbox->setChecked(TheoreticalDiary::instance()->settings->value("sync_enabled", false).toBool());
}

void OptionsMenu::export_diary()
{
    auto const &filename = QFileDialog::getSaveFileName(
        this, "Export diary", QString("%1/export.json").arg(QDir::homePath()), "JSON (*.json);;All files");

    if (filename.isEmpty())
        return;

    std::ofstream dst(filename.toStdString());

    if (!dst.fail()) {
        nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);
        dst << j.dump(4);
        dst.close();

        td::ok_messagebox(this, "Diary exported.", "The diary has been exported in an unencrypted JSON format.");
    }
    else {
        td::ok_messagebox(this, "Export failed.", "The app could not write to the specified location.");
    }
}

void OptionsMenu::change_password()
{
    ui->change_password_button->setEnabled(false);
    ui->apply_button->setEnabled(false);
    ui->ok_button->setEnabled(false);
    TheoreticalDiary::instance()->closeable = false;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->alert_text->setText("");
    ui->alert_text->update();

    auto const &password = ui->new_password->text();
    if (password != ui->new_password_confirm->text()) {
        ui->alert_text->setText("The passwords do not match.");
        ui->alert_text->update();

        QApplication::restoreOverrideCursor();
        TheoreticalDiary::instance()->closeable = true;
        ui->apply_button->setEnabled(true);
        ui->ok_button->setEnabled(true);
        ui->change_password_button->setEnabled(true);
    }

    if (0 != password.length()) {
        ui->alert_text->setText("Changing password...");
        ui->alert_text->update();

        auto worker = new HashWorker();
        worker->moveToThread(&TheoreticalDiary::instance()->worker_thread);
        connect(&TheoreticalDiary::instance()->worker_thread, &QThread::finished, worker, &QObject::deleteLater,
            Qt::QueuedConnection);
        connect(worker, &HashWorker::done, this, &OptionsMenu::change_password_cb, Qt::QueuedConnection);
        connect(TheoreticalDiary::instance(), &TheoreticalDiary::sig_begin_hash, worker, &HashWorker::hash,
            Qt::QueuedConnection);

        TheoreticalDiary::instance()->encryptor->regenerate_salt();
        emit TheoreticalDiary::instance()->sig_begin_hash(password.toStdString());
    }
    else {
        TheoreticalDiary::instance()->encryptor->reset();
        change_password_cb();
    }
}

void OptionsMenu::change_password_cb()
{
    TheoreticalDiary::instance()->diary_changed();
    QApplication::restoreOverrideCursor();
    ui->alert_text->setText("Password updated.");
    TheoreticalDiary::instance()->closeable = true;
    ui->apply_button->setEnabled(true);
    ui->ok_button->setEnabled(true);
    ui->change_password_button->setEnabled(true);
}

void OptionsMenu::request_start()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    TheoreticalDiary::instance()->closeable = false;
    ui->download_backup_button->setEnabled(false);
    ui->upload_backup_button->setEnabled(false);
    ui->flush_oauth_button->setEnabled(false);
}

void OptionsMenu::request_end()
{
    ui->download_backup_button->setEnabled(true);
    ui->upload_backup_button->setEnabled(true);
    ui->flush_oauth_button->setEnabled(true);
    TheoreticalDiary::instance()->closeable = true;
    QApplication::restoreOverrideCursor();
}

void OptionsMenu::download_backup()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->download_diary(this);
}

void OptionsMenu::upload_diary()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->upload_diary(this, false);
}

void OptionsMenu::flush_oauth()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();
    TheoreticalDiary::instance()->gwrapper->revoke_access();
    // According to the docs, this function is always successful.
    TheoreticalDiary::instance()->gwrapper->google->unlink();

    request_end();
    td::ok_messagebox(this, "Credentials deleted.", "The OAuth2 credentials have been deleted.");
}

void OptionsMenu::dev_list()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        // This makes sure that this lambda is only executed once. Consider the following situation:
        // A request is made with an invalid token. o2 only checks IF a token exists when authenticate() is called
        // below, it does not check if it actually works. This means that the code returned is actually td::Res::Yes
        // which means that list_files() is called. However, this time o2 realises the token is invalid because Google
        // sends back a 401 code. When the token is automatically refreshed by o2, the sig_oauth2_callback signal is
        // emitted since the client is technically in a linked state again. This means td::Res::Yes is returned AGAIN
        // and the code below is run again, meaning list_files() is called AGAIN. This is problematic as the original
        // list_files() request goes through as well. So far, I have found that this bug causes the upload_file()
        // function to throw a segmentation fault for some reason (please don't ask why). Anyway, this bug took a while
        // to pin down since the stack traces of both list_files() calls were basically the same (and because of the
        // fact that I did not know o2 called sig_oauth2_callback when the access token is refreshed).
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // List all files in drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                request_end();
                r.exec();
            });

        TheoreticalDiary::instance()->gwrapper->list_files();
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_unknown_file()
{
    td::ok_messagebox(this, "Access failed.", "The app could not access the specified location.");
}

void OptionsMenu::dev_upload()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Upload file to drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                request_end();
                r.exec();
            });

        auto const &filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
        if (filename.isEmpty())
            return request_end();

        QFile f(filename);
        QFileInfo fi(f);

        auto const &res = TheoreticalDiary::instance()->gwrapper->upload_file(filename, fi.fileName());
        if (td::Res::No == res) {
            request_end();
            dev_unknown_file();
        }
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_download()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Download file from drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const error, QByteArray data) {
                request_end();

                if (QNetworkReply::NoError != error) {
                    APIResponse r(data, this);
                    r.exec();
                    return;
                }

                auto const &filename =
                    QFileDialog::getSaveFileName(this, "Download file", QString("%1/download").arg(QDir::homePath()));
                if (filename.isEmpty())
                    return request_end();

                QFile file(filename);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
                    return dev_unknown_file();
                file.write(data);
                file.close();

                QByteArray res("File downloaded.");
                APIResponse r(res, this);
                r.exec();
            });

        TheoreticalDiary::instance()->gwrapper->download_file(ui->dev_download_file_id->text());
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_update()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Upload file to drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                request_end();
                r.exec();
            });

        auto const &filename = QFileDialog::getOpenFileName(this, "Update file", QDir::homePath());
        if (filename.isEmpty())
            return request_end();

        auto const &id = ui->dev_update_file_id->text();
        auto const &res = TheoreticalDiary::instance()->gwrapper->update_file(id, filename);
        if (td::Res::No == res) {
            request_end();
            dev_unknown_file();
        }
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_copy()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Copy file on drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                request_end();
                r.exec();
            });

        TheoreticalDiary::instance()->gwrapper->copy_file(
            ui->dev_copy_file_id->text(), ui->dev_copy_file_new_name->text());
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_delete()
{
    request_start();
    TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_oauth2_callback, [this](td::Res const code) {
        TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

        if (td::Res::No == code) {
            request_end();
            return TheoreticalDiary::instance()->gwrapper->display_auth_error(this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Delete file on drive.
        connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const error, QByteArray data) {
                request_end();

                if (QNetworkReply::NoError != error) {
                    APIResponse r(data, this);
                    r.exec();
                    return;
                }

                QByteArray res("File deleted.");
                APIResponse r(res, this);
                r.exec();
            });

        TheoreticalDiary::instance()->gwrapper->delete_file(ui->dev_delete_file_id->text());
    });

    TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::show_about()
{
    AboutDialog w(this);
    w.exec();
}

void OptionsMenu::show_licenses()
{
    LicensesDialog w(this);
    w.exec();
}
