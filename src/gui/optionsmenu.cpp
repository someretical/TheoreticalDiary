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

#include <fstream>

#include "../core/diaryholder.h"
#include "../core/googlewrapper.h"
#include "../core/internalmanager.h"
#include "../util/custommessageboxes.h"
#include "../util/hashcontroller.h"
#include "aboutdialog.h"
#include "apiresponse.h"
#include "licensesdialog.h"
#include "mainwindow.h"
#include "optionsmenu.h"
#include "ui_optionsmenu.h"

OptionsMenu::OptionsMenu(bool const from_diary_editor, QWidget *parent) : QWidget(parent), ui(new Ui::OptionsMenu)
{
    ui->setupUi(this);

    diary_editor_mode = from_diary_editor;

    connect(ui->ok_button, &QPushButton::clicked, this, &OptionsMenu::back, Qt::QueuedConnection);
    connect(ui->apply_button, &QPushButton::clicked, this, &OptionsMenu::save_settings, Qt::QueuedConnection);
    connect(ui->export_button, &QPushButton::clicked, this, &OptionsMenu::export_diary, Qt::QueuedConnection);
    connect(
        ui->update_lock_timeout, &QPushButton::clicked, this, &OptionsMenu::update_lock_timeout, Qt::QueuedConnection);
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
    connect(ui->reset_button, &QPushButton::clicked, this, &OptionsMenu::reset_settings, Qt::QueuedConnection);
    connect(ui->test_button, &QPushButton::clicked, this, &OptionsMenu::test, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &OptionsMenu::update_theme,
        Qt::QueuedConnection);
    update_theme();

    setup_layout();
}

OptionsMenu::~OptionsMenu()
{
    delete ui;
}

void OptionsMenu::update_theme()
{
    auto const &theme = InternalManager::instance()->get_theme_str();

    QFile file(QString(":/%1/optionsmenu.qss").arg(theme));
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}

void OptionsMenu::back()
{
    MainWindow::instance()->show_main_menu(false);
}

void OptionsMenu::save_settings()
{
    auto settings = InternalManager::instance()->settings;

    settings->setValue("sync_enabled", ui->sync_checkbox->isChecked());

    auto original_theme = InternalManager::instance()->get_theme();
    auto new_theme = ui->theme_dropdown->currentIndex() == 0 ? td::Theme::Dark : td::Theme::Light;
    if (original_theme != new_theme) {
        settings->setValue("theme", static_cast<int>(new_theme));
        InternalManager::instance()->update_theme();
    }

    qDebug() << "Saved settings.";
}

void OptionsMenu::setup_layout()
{
    ui->pwd_alert_text->set_text("");
    ui->lock_timeout_text->set_text("");

    auto const &settings = InternalManager::instance()->settings;

    auto theme = static_cast<td::Theme>(settings->value("theme").toInt());
    ui->theme_dropdown->setCurrentIndex(td::Theme::Dark == theme ? 0 : 1);

    ui->lock_timeout_textedit->setText(QString::number(settings->value("lock_timeout").toLongLong()));

    if (!diary_editor_mode) {
        ui->export_button->setEnabled(false);
        ui->lock_timeout_textedit->setEnabled(false);
        ui->update_lock_timeout->setEnabled(false);
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
    else {
        ui->download_backup_button->setEnabled(false);
        ui->ok_button->setVisible(false);
    }

    ui->sync_checkbox->setChecked(InternalManager::instance()->settings->value("sync_enabled").toBool());
}

void OptionsMenu::export_diary()
{
    auto const &filename = QFileDialog::getSaveFileName(
        this, "Export diary", QString("%1/export.json").arg(QDir::homePath()), "JSON (*.json);;All files");

    if (filename.isEmpty())
        return;

    std::ofstream dst(filename.toStdString());

    if (!dst.fail()) {
        InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
        nlohmann::json const &j = DiaryHolder::instance()->diary;
        dst << j.dump(4);

        td::ok_messagebox(this, "Diary exported.", "The diary has been exported in an unencrypted JSON format.");
    }
    else {
        td::ok_messagebox(this, "Export failed.", "The app could not write to the specified location.");
    }
}

void OptionsMenu::update_lock_timeout()
{
    bool ok = true;
    qint64 const &ms = ui->lock_timeout_textedit->text().toLongLong(&ok, 10);

    if (!ok || ms < 0)
        return ui->lock_timeout_text->set_text("Please provide a positive integer.");

    InternalManager::instance()->settings->setValue("lock_timeout", ms);
    InternalManager::instance()->inactive_filter->interval = ms;
    InternalManager::instance()->inactive_filter->timer->start();

    ui->lock_timeout_text->set_text("Lock timeout updated.");
}

void OptionsMenu::change_password()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);

    ui->pwd_alert_text->set_text("");

    auto const &password = ui->new_password->text();
    if (password != ui->new_password_confirm->text()) {
        ui->pwd_alert_text->set_text("The passwords do not match.");

        return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    }

    if (0 != password.length()) {
        ui->pwd_alert_text->set_text("Changing password...", false);
        ui->new_password->setText("");
        ui->new_password->update();
        ui->new_password_confirm->setText("");
        ui->new_password_confirm->update();

        auto hash_controller = new HashController();
        connect(
            hash_controller, &HashController::sig_done, this, &OptionsMenu::change_password_cb, Qt::QueuedConnection);
        connect(hash_controller, &HashController::sig_delete, hash_controller, &HashController::deleteLater,
            Qt::QueuedConnection);
        emit hash_controller->operate(password.toStdString());
    }
    else {
        qDebug() << "Resetting password to nothing.";
        Encryptor::instance()->reset();
        change_password_cb(false);
    }
}

void OptionsMenu::change_password_cb(bool const)
{
    InternalManager::instance()->internal_diary_changed = true;
    ui->pwd_alert_text->set_text("Password updated.");
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
}

void OptionsMenu::download_backup()
{
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    if (!cmb::prompt_confirm_overwrite(this))
        return;

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto check_error = [this, intman](td::NR const &reply) {
        if (QNetworkReply::NoError != reply.error) {
            td::ok_messagebox(this, "Network error.", reply.error_message.toStdString());
            return false;
        }

        return true;
    };

    auto cb_final = [this, check_error, intman](td::NR const &reply) {
        if (!check_error(reply))
            return;

        QFile file(QString("%1/diary.dat").arg(intman->data_location()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return cmb::display_io_error(this);

        // At the moment, the data is written all in one go as o2 does not yet provide any download updates. This is
        // really bad practice so TODO: add own implementation of a download notifier so the data buffer can be
        // piped into the file every time a new chunk arrives.
        file.write(reply.response);
        cmb::diary_downloaded(this);
    };

    auto cb2 = [this, check_error, gwrapper, intman, cb_final](td::NR const &reply) {
        if (!check_error(reply))
            return;

        auto const &[id1, id2] = gwrapper->get_file_ids(reply.response);

        if (id1.isEmpty() && id2.isEmpty())
            return cmb::display_drive_file_error(this);

        gwrapper->download_file(id1.isEmpty() ? id2 : id1).subscribe(cb_final);
    };

    auto cb1 = [this, gwrapper, intman, cb2]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->list_files().subscribe(cb2);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::upload_diary()
{
    static QString primary_id;
    static QString secondary_id;
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto check_error = [this, intman](td::NR const &reply) {
        if (QNetworkReply::NoError != reply.error) {
            td::ok_messagebox(this, "Network error.", reply.error_message.toStdString());
            return false;
        }

        return true;
    };

    auto cb_final = [this, check_error, gwrapper, intman](td::NR const &reply) {
        if (check_error(reply))
            cmb::diary_uploaded(this);
    };

    auto upload_subroutine = [this, gwrapper, intman, cb_final]() {
        auto file_ptr = new QFile(QString("%1/diary.dat").arg(intman->data_location()));

        if (!file_ptr->open(QIODevice::ReadOnly))
            return cmb::display_io_error(this);

        if (primary_id.isEmpty())
            gwrapper->upload_file(file_ptr, "diary.dat").subscribe(cb_final);
        else
            gwrapper->update_file(file_ptr, primary_id).subscribe(cb_final);
    };

    auto cb4 = [this, check_error, upload_subroutine, gwrapper, intman, cb_final](td::NR const &reply) {
        if (!check_error(reply))
            return;

        upload_subroutine();
    };

    auto cb3 = [this, check_error, upload_subroutine, gwrapper, intman, cb4, cb_final](td::NR const &reply) {
        if (!check_error(reply))
            return;

        if (secondary_id.isEmpty())
            return upload_subroutine();

        gwrapper->delete_file(secondary_id).subscribe(cb4);
    };

    auto cb2 = [this, check_error, upload_subroutine, gwrapper, intman, cb3, cb_final](td::NR const &reply) {
        if (!check_error(reply))
            return;

        auto const &[id1, id2] = gwrapper->get_file_ids(reply.response);

        if (id1.isEmpty())
            return upload_subroutine();

        primary_id = id1;
        secondary_id = id2;
        gwrapper->copy_file(id1, "diary.dat.bak").subscribe(cb3);
    };

    auto cb1 = [this, gwrapper, intman, cb2]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->list_files().subscribe(cb2);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::flush_oauth()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    auto gwrapper = GoogleWrapper::instance();

    auto cb = [this, gwrapper](td::NR const &) {
        gwrapper->google->unlink();
        td::ok_messagebox(this, "Credentials deleted.", "The OAuth2 credentials have been deleted.");
    };

    gwrapper->revoke_access().subscribe(cb);
}

void OptionsMenu::dev_list()
{
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r(reply.response, this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->list_files().subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::dev_upload()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
    if (filename.isEmpty())
        return;

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r(reply.response, this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            auto file_ptr = new QFile(filename);

            if (!file_ptr->open(QIODevice::ReadOnly))
                return cmb::display_io_error(this);

            QFileInfo fileinfo(file_ptr->fileName());
            gwrapper->upload_file(file_ptr, fileinfo.fileName()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::dev_download()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    filename = QFileDialog::getSaveFileName(this, "Download file", QString("%1/download").arg(QDir::homePath()));
    if (filename.isEmpty())
        return;

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        if (QNetworkReply::NoError != reply.error) {
            misc::clear_message_boxes();
            intman->end_busy_mode(__LINE__, __func__, __FILE__);
            APIResponse r(reply.response, this);
            r.exec();
            return;
        }

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return cmb::display_io_error(this);
        file.write(reply.response);

        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r("File downloaded.", this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->download_file(ui->dev_download_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::dev_update()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
    if (filename.isEmpty())
        return;

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r(reply.response, this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            auto file_ptr = new QFile(filename);

            if (!file_ptr->open(QIODevice::ReadOnly))
                return cmb::display_io_error(this);

            gwrapper->update_file(file_ptr, ui->dev_update_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::dev_copy()
{
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r(reply.response, this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->copy_file(ui->dev_copy_file_id->text(), ui->dev_copy_file_new_name->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
}

void OptionsMenu::dev_delete()
{
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    intman->start_busy_mode(__LINE__, __func__, __FILE__);

    auto cb_final = [this, intman](td::NR const &reply) {
        misc::clear_message_boxes();
        intman->end_busy_mode(__LINE__, __func__, __FILE__);
        APIResponse r(
            QNetworkReply::NoError != reply.error ? reply.error_message.toLocal8Bit() : "File deleted.", this);
        r.exec();
    };

    auto cb1 = [this, gwrapper, intman, cb_final]() {
        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            gwrapper->delete_file(ui->dev_delete_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    gwrapper->google->link();
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

void OptionsMenu::reset_settings()
{
    int res =
        td::yn_messagebox(this, "Are you sure you want to reset all the settings?", "This action cannot be undone!");

    if (QMessageBox::AcceptRole == res) {
        InternalManager::instance()->init_settings(true);
        td::ok_messagebox(this, "Settings reset.", "Please restart the app for the changes to take effect.");
    }
}

void OptionsMenu::test() {}
