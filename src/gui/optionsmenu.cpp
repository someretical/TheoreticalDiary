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
#include <utility>

#include "../core/diaryholder.h"
#include "../core/googlewrapper.h"
#include "../core/internalmanager.h"
#include "../util/custommessageboxes.h"
#include "../util/hashcontroller.h"
#include "../util/misc.h"
#include "aboutdialog.h"
#include "apiresponse.h"
#include "mainwindow.h"
#include "o2google.h"
#include "optionsmenu.h"
#include "ui_optionsmenu.h"

OptionsMenu::OptionsMenu(bool const from_diary_editor, QWidget *parent) : QWidget(parent), ui(new Ui::OptionsMenu)
{
    ui->setupUi(this);

    ui->pie_slice_sorting->setId(ui->pie_slice_sort1, static_cast<int>(td::settings::PieSliceSort::Days));
    ui->pie_slice_sorting->setId(ui->pie_slice_sort2, static_cast<int>(td::settings::PieSliceSort::Category));

    diary_editor_mode = from_diary_editor;

    connect(ui->ok_button, &QPushButton::clicked, this, &OptionsMenu::back, Qt::QueuedConnection);
    connect(ui->apply_button, &QPushButton::clicked, this, &OptionsMenu::save_settings, Qt::QueuedConnection);
    connect(ui->export_button, &QPushButton::clicked, this, &OptionsMenu::export_diary, Qt::QueuedConnection);
    connect(
        ui->update_lock_timeout, &QPushButton::clicked, this, &OptionsMenu::update_lock_timeout, Qt::QueuedConnection);
    connect(
        ui->change_password_button, &QPushButton::clicked, this, &OptionsMenu::change_password, Qt::QueuedConnection);
    connect(ui->auth_button, &QPushButton::clicked, this, &OptionsMenu::complete_oauth, Qt::QueuedConnection);
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
    connect(ui->reset_button, &QPushButton::clicked, this, &OptionsMenu::reset_settings, Qt::QueuedConnection);
    connect(ui->test_button, &QPushButton::clicked, this, &OptionsMenu::test, Qt::QueuedConnection);

    connect(GoogleWrapper::instance()->google, &O2Google::linkedChanged, this, &OptionsMenu::refresh_linked_checkbox,
        Qt::QueuedConnection);

    setup_layout();
}

OptionsMenu::~OptionsMenu()
{
    delete ui;
}

void OptionsMenu::back()
{
    MainWindow::instance()->show_main_menu(false);
}

void OptionsMenu::save_settings()
{
    auto settings = InternalManager::instance()->settings;

    settings->setValue("sync_enabled", ui->sync_checkbox->isChecked());
    settings->setValue("pie_slice_sort", ui->pie_slice_sorting->checkedId());

    //    auto original_theme = InternalManager::instance()->get_theme();
    auto new_theme = ui->theme_dropdown->currentIndex() == 0 ? td::Theme::Dark : td::Theme::Light;
    //    if (original_theme != new_theme) {
    //        settings->setValue("theme", static_cast<int>(new_theme));
    //        InternalManager::instance()->start_update_theme();
    //    }
    settings->setValue("theme", static_cast<int>(new_theme));

    // This workaround has to be put in place to make sure that the diary is uploaded if the user decrypts it, ticks the
    // backup option, and closes the diary without making any other changes.
    if (ui->sync_checkbox->isChecked())
        InternalManager::instance()->diary_file_changed = true;

    InternalManager::instance()->start_update_theme();

    qDebug() << "Saved settings.";
}

void OptionsMenu::setup_layout()
{
    ui->pwd_alert_text->set_text("");
    ui->lock_timeout_text->set_text("");

    auto const &settings = InternalManager::instance()->settings;

    auto theme = static_cast<td::Theme>(settings->value("theme").toInt());
    ui->theme_dropdown->setCurrentIndex(td::Theme::Dark == theme ? 0 : 1);

    ui->oauth_checkbox->setEnabled(false);
    ui->lock_timeout_textedit->setText(QString::number(settings->value("lock_timeout").toLongLong()));
    ui->sync_checkbox->setChecked(settings->value("sync_enabled").toBool());
    qobject_cast<QRadioButton *>(ui->pie_slice_sorting->button(settings->value("pie_slice_sort").toInt()))
        ->setChecked(true);

    if (!diary_editor_mode) {
        ui->export_button->setEnabled(false);
        ui->lock_timeout_textedit->setEnabled(false);
        ui->update_lock_timeout->setEnabled(false);
        ui->change_password_button->setEnabled(false);
        ui->new_password->setEnabled(false);
        ui->new_password_confirm->setEnabled(false);
        ui->dev_list_files_button->setEnabled(false);
        ui->dev_upload_file_button->setEnabled(false);
        ui->dev_download_file_button->setEnabled(false);
        ui->dev_update_file_button->setEnabled(false);
        ui->dev_copy_file_button->setEnabled(false);
        ui->dev_delete_button->setEnabled(false);
    }
    else {
        ui->ok_button->setVisible(false);
    }

    refresh_linked_checkbox();
}

void OptionsMenu::refresh_linked_checkbox()
{
    if (GoogleWrapper::instance()->google->linked()) {
        ui->oauth_checkbox->setCheckState(Qt::Checked);
        ui->flush_oauth_button->setEnabled(true);

        if (diary_editor_mode) {
            ui->upload_backup_button->setEnabled(true);
            ui->download_backup_button->setEnabled(false);
        }
        else {
            ui->upload_backup_button->setEnabled(false);
            ui->download_backup_button->setEnabled(true);
        }
    }
    else {
        ui->oauth_checkbox->setCheckState(Qt::Unchecked);
        ui->download_backup_button->setEnabled(false);
        ui->upload_backup_button->setEnabled(false);
        ui->flush_oauth_button->setEnabled(false);
    }
}

void OptionsMenu::export_diary()
{
    auto const &filename = QFileDialog::getSaveFileName(
        this, "Export diary", QString("%1/export.json").arg(QDir::homePath()), "JSON (*.json);;All files");

    if (filename.isEmpty())
        return;

    AppBusyLock lock;
    std::ofstream dst(filename.toStdString());

    if (dst.fail())
        return cmb::display_local_io_error(this);

    nlohmann::json const &j = DiaryHolder::instance()->diary;
    dst << j.dump(4);

    auto msgbox = new QMessageBox(this);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The diary has been exported in an unencrypted JSON format.");
    msgbox->setInformativeText("This means anybody can read it!");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void OptionsMenu::update_lock_timeout()
{
    AppBusyLock lock;
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
    AppBusyLock lock(true);
    ui->pwd_alert_text->set_text("");

    auto const &password = ui->new_password->text();
    if (password != ui->new_password_confirm->text()) {
        ui->pwd_alert_text->set_text("The passwords do not match.");

        return lock.release();
    }

    auto cb = [this](bool const) {
        AppBusyLock lock;
        InternalManager::instance()->internal_diary_changed = true;
        ui->pwd_alert_text->set_text("Password updated.");
    };

    if (0 != password.length()) {
        ui->pwd_alert_text->set_text("Changing password...", false);
        ui->new_password->setText("");
        ui->new_password->update();
        ui->new_password_confirm->setText("");
        ui->new_password_confirm->update();

        auto hash_controller = new HashController();
        connect(hash_controller, &HashController::sig_done, cb);
        connect(hash_controller, &HashController::sig_delete, hash_controller, &HashController::deleteLater,
            Qt::QueuedConnection);
        emit hash_controller->operate(password.toStdString());
    }
    else {
        qDebug() << "Resetting password to nothing.";
        Encryptor::instance()->reset();
        cb(false);
    }
}

void OptionsMenu::complete_oauth()
{
    auto gwrapper = GoogleWrapper::instance();

    qDebug() << "User attempted to link Google account...";

    auto cb1 = [this, gwrapper]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            auto msgbox = new QMessageBox(this);
            msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
            msgbox->setText("Successfully linked Google account.");
            msgbox->setStandardButtons(QMessageBox::Ok);
            msgbox->show();
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::download_backup()
{
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    auto check_error = [this](td::NR const &reply) {
        if (QNetworkReply::NoError != reply.error) {
            cmb::display_google_drive_network_error(this, std::move(reply.error_message));
            return false;
        }

        return true;
    };

    auto cb_final = [this, check_error, intman](td::NR const &reply) {
        AppBusyLock lock;

        if (!check_error(reply))
            return;

        QFile file(QString("%1/diary.dat").arg(intman->data_location()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return cmb::display_local_diary_access_error(this);

        // At the moment, the data is written all in one go as o2 does not yet provide any download updates. This is
        // really bad practice so TODO: add own implementation of a download notifier so the data buffer can be
        // piped into the file every time a new chunk arrives.
        file.write(reply.response);
        cmb::diary_downloaded(this);
    };

    auto cb2 = [this, check_error, gwrapper, cb_final](td::NR const &reply) {
        if (!check_error(reply))
            return;

        auto const &[id1, id2] = gwrapper->get_file_ids(reply.response);

        if (id1.isEmpty() && id2.isEmpty())
            return cmb::display_google_drive_missing_file(this);

        gwrapper->download_file(id1.isEmpty() ? id2 : id1).subscribe(cb_final);
    };

    auto cb1 = [this, gwrapper, cb2]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->list_files().subscribe(cb2);
            break;
        }
    };

    auto cb = [gwrapper, cb1](int const res) {
        if (QMessageBox::No == res)
            return;

        AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
        AppBusyLock lock(true);
        gwrapper->google->link();
    };

    cmb::prompt_diary_overwrite(this, cb);
}

void OptionsMenu::upload_diary()
{
    static QString primary_id;
    static QString secondary_id;
    auto gwrapper = GoogleWrapper::instance();
    auto intman = InternalManager::instance();

    auto check_error = [this](td::NR const &reply) {
        if (QNetworkReply::NoError != reply.error) {
            AppBusyLock lock;
            cmb::display_google_drive_network_error(this, std::move(reply.error_message));
            return false;
        }

        return true;
    };

    auto cb_final = [this, check_error, gwrapper](td::NR const &reply) {
        AppBusyLock lock;

        if (check_error(reply))
            cmb::diary_uploaded(this);
    };

    auto upload_subroutine = [this, gwrapper, intman, cb_final]() {
        auto file_ptr = new QFile(QString("%1/diary.dat").arg(intman->data_location()));

        if (!file_ptr->open(QIODevice::ReadOnly)) {
            AppBusyLock lock;
            return cmb::display_local_diary_access_error(this);
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

    auto cb1 = [this, gwrapper, cb2]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->list_files().subscribe(cb2);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::flush_oauth()
{
    auto gwrapper = GoogleWrapper::instance();

    auto cb = [this, gwrapper](td::NR const &) {
        AppBusyLock lock;

        gwrapper->google->unlink();

        auto msgbox = new QMessageBox(this);
        msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
        msgbox->setText("The OAuth2 tokens have been deleted.");
        msgbox->setStandardButtons(QMessageBox::Ok);
        msgbox->show();
    };

    AppBusyLock lock(true);
    gwrapper->revoke_access().subscribe(cb);
}

void OptionsMenu::dev_list()
{
    auto gwrapper = GoogleWrapper::instance();

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        misc::clear_message_boxes();
        auto r = new APIResponse(reply.response, this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->list_files().subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::dev_upload()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();

    filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
    if (filename.isEmpty())
        return;

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        misc::clear_message_boxes();
        auto r = new APIResponse(reply.response, this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            auto file_ptr = new QFile(filename);

            if (!file_ptr->open(QIODevice::ReadOnly))
                return cmb::display_local_io_error(this);

            lock.persist = true;
            QFileInfo fileinfo(file_ptr->fileName());
            gwrapper->upload_file(file_ptr, fileinfo.fileName()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::dev_download()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();

    filename = QFileDialog::getSaveFileName(this, "Download file", QString("%1/download").arg(QDir::homePath()));
    if (filename.isEmpty())
        return;

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        if (QNetworkReply::NoError != reply.error) {
            misc::clear_message_boxes();
            auto r = new APIResponse(reply.response, this);
            r->setAttribute(Qt::WA_DeleteOnClose, true);
            return r->show();
        }

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return cmb::display_local_io_error(this);
        file.write(reply.response);

        misc::clear_message_boxes();
        auto r = new APIResponse("File downloaded.", this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        return r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->download_file(ui->dev_download_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::dev_update()
{
    static QString filename;
    auto gwrapper = GoogleWrapper::instance();

    filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
    if (filename.isEmpty())
        return;

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        misc::clear_message_boxes();
        auto r = new APIResponse(reply.response, this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            auto file_ptr = new QFile(filename);

            if (!file_ptr->open(QIODevice::ReadOnly))
                return cmb::display_local_io_error(this);

            lock.persist = true;
            gwrapper->update_file(file_ptr, ui->dev_update_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::dev_copy()
{
    auto gwrapper = GoogleWrapper::instance();

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        misc::clear_message_boxes();
        auto r = new APIResponse(reply.response, this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->copy_file(ui->dev_copy_file_id->text(), ui->dev_copy_file_new_name->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::dev_delete()
{
    auto gwrapper = GoogleWrapper::instance();

    auto cb_final = [this](td::NR const &reply) {
        AppBusyLock lock;

        misc::clear_message_boxes();
        auto r = new APIResponse(
            QNetworkReply::NoError != reply.error ? reply.error_message.toLocal8Bit() : "File deleted.", this);
        r->setAttribute(Qt::WA_DeleteOnClose, true);
        r->show();
    };

    auto cb1 = [this, gwrapper, cb_final]() {
        AppBusyLock lock;

        switch (gwrapper->verify_auth()) {
        case td::LinkingResponse::ScopeMismatch:
            cmb::display_google_drive_scope_mismatch(this);
            break;
        case td::LinkingResponse::Fail:
            cmb::display_google_drive_auth_error(this);
            break;
        case td::LinkingResponse::OK:
            lock.persist = true;
            gwrapper->delete_file(ui->dev_delete_file_id->text()).subscribe(cb_final);
            break;
        }
    };

    AsyncFuture::observe(gwrapper->google, &O2Google::linkingDone).subscribe(cb1);
    AppBusyLock lock(true);
    gwrapper->google->link();
}

void OptionsMenu::show_about()
{
    AboutDialog w(this);
    w.exec();
}

void OptionsMenu::reset_settings()
{
    auto cb = [this](int const res) {
        if (QMessageBox::No == res)
            return;

        InternalManager::instance()->init_settings(true);

        auto msgbox = new QMessageBox(this);
        msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
        msgbox->setText("Please restart the app for the changes to take effect.");
        msgbox->setStandardButtons(QMessageBox::Ok);
        msgbox->show();
    };

    auto msgbox = new QMessageBox(this);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("Are you sure you want to reset all the settings?");
    msgbox->setInformativeText("This action cannot be undone!");
    msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox->setDefaultButton(QMessageBox::No);
    QObject::connect(msgbox, &QMessageBox::finished, cb);
    msgbox->show();
}

void OptionsMenu::test() {}
