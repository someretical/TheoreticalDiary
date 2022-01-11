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
    if (!diary_editor_mode)
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
    back();
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
        InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

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
    if (!cmb::prompt_confirm_overwrite(this))
        return;

    GoogleWrapper::instance()->download_diary([this](const td::GWrapperError err) {
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
            cmb::diary_downloaded(this);
            break;
        }
    });
}

void OptionsMenu::upload_diary()
{
    GoogleWrapper::instance()->upload_diary([this](const td::GWrapperError err) {
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

void OptionsMenu::flush_oauth()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_requestor_slots();
    GoogleWrapper::instance()->revoke_access();
    // According to the docs, this function is always successful.
    GoogleWrapper::instance()->google->unlink();

    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    td::ok_messagebox(this, "Credentials deleted.", "The OAuth2 credentials have been deleted.");
}

void OptionsMenu::dev_list()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        // This makes sure that this lambda is only executed once. Consider the following situation:
        // A request is made with an invalid token. o2 only checks IF a token exists when authenticate() is called
        // below, it does not check if it actually works. This means that it is successful which means that list_files()
        // is called. However, this time o2 realises the token is invalid because Google sends back a 401 code. When the
        // token is automatically refreshed by o2, the sig_oauth2_callback signal is emitted since the client is
        // technically in a linked state again. This means true is returned AGAIN and the code below is run again,
        // meaning list_files() is called AGAIN. This is problematic as the original list_files() request goes through
        // as well. So far, I have found that this bug causes the upload_file() function to throw a segmentation fault
        // for some reason (please don't ask why). Anyway, this bug took a while to pin down since the stack traces of
        // both list_files() calls were basically the same (and because of the fact that I did not know o2 called
        // sig_oauth2_callback when the access token is refreshed).
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // List all files in drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
                r.exec();
            });

        GoogleWrapper::instance()->list_files();
    });

    GoogleWrapper::instance()->authenticate();
}

void OptionsMenu::dev_upload()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // Upload file to drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
                r.exec();
            });

        auto const &filename = QFileDialog::getOpenFileName(this, "Upload file", QDir::homePath());
        if (filename.isEmpty())
            return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

        QFile f(filename);
        QFileInfo fi(f);

        auto const success2 = GoogleWrapper::instance()->upload_file(filename, fi.fileName());
        if (!success2) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            cmb::dev_unknown_file(this);
        }
    });

    GoogleWrapper::instance()->authenticate();
}

void OptionsMenu::dev_download()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // Download file from drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const error, QByteArray data) {
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

                if (QNetworkReply::NoError != error) {
                    APIResponse r(data, this);
                    r.exec();
                    return;
                }

                auto const &filename =
                    QFileDialog::getSaveFileName(this, "Download file", QString("%1/download").arg(QDir::homePath()));
                if (filename.isEmpty())
                    return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

                QFile file(filename);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
                    return cmb::dev_unknown_file(this);
                file.write(data);

                QByteArray res("File downloaded.");
                APIResponse r(res, this);
                r.exec();
            });

        GoogleWrapper::instance()->download_file(ui->dev_download_file_id->text());
    });

    GoogleWrapper::instance()->authenticate();
}

void OptionsMenu::dev_update()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // Upload file to drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
                r.exec();
            });

        auto const &filename = QFileDialog::getOpenFileName(this, "Update file", QDir::homePath());
        if (filename.isEmpty())
            return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

        auto const &id = ui->dev_update_file_id->text();
        auto const success2 = GoogleWrapper::instance()->update_file(id, filename);
        if (!success2) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            cmb::dev_unknown_file(this);
        }
    });

    GoogleWrapper::instance()->authenticate();
}

void OptionsMenu::dev_copy()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // Copy file on drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const, QByteArray data) {
                APIResponse r(data, this);
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
                r.exec();
            });

        GoogleWrapper::instance()->copy_file(ui->dev_copy_file_id->text(), ui->dev_copy_file_new_name->text());
    });

    GoogleWrapper::instance()->authenticate();
}

void OptionsMenu::dev_delete()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    GoogleWrapper::instance()->dc_oauth_slots();

    // Check for OAuth2 credentials first.
    connect(GoogleWrapper::instance(), &GoogleWrapper::sig_oauth2_callback, [this](bool const success) {
        GoogleWrapper::instance()->dc_oauth_slots();

        if (!success) {
            InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            return cmb::display_auth_error(this);
        }

        GoogleWrapper::instance()->dc_requestor_slots();

        // Delete file on drive.
        connect(GoogleWrapper::instance()->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished),
            [this](int const, QNetworkReply::NetworkError const error, QByteArray data) {
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

                if (QNetworkReply::NoError != error) {
                    APIResponse r(data, this);
                    r.exec();
                    return;
                }

                QByteArray res("File deleted.");
                APIResponse r(res, this);
                r.exec();
            });

        GoogleWrapper::instance()->delete_file(ui->dev_delete_file_id->text());
    });

    GoogleWrapper::instance()->authenticate();
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

    switch (res) {
    case QMessageBox::AcceptRole:
        InternalManager::instance()->init_settings(true);
        td::ok_messagebox(this, "Settings reset.", "Please restart the app for the changes to take effect.");
        back();
        break;
    case QMessageBox::RejectRole:
        break;
    }
}
