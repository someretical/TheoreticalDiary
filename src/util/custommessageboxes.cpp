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

#include "custommessageboxes.h"

namespace cmb {
void display_local_io_error(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app could not access the specified location.");
    msgbox->setInformativeText("Please make sure it has sufficient permissions.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_local_diary_save_error(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app could not save the diary.");
    msgbox->setInformativeText("Please make sure it has sufficient permissions.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_google_drive_network_error(QWidget *p, QString const &&err)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app encountered a network error while communicating with Google Drive.");
    msgbox->setInformativeText("Error: " + err);
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void diary_uploaded(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The diary has been uploaded to Google Drive.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void diary_downloaded(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The diary has been downloaded from Google Drive.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_local_diary_access_error(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app could not access the local diary.");
    msgbox->setInformativeText("Please make sure it has sufficient permissions.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_google_drive_scope_mismatch(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app is missing required Google OAuth scopes.");
    msgbox->setInformativeText("Please reauthorize and grant them all.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_google_drive_auth_error(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app could authenticate with Google Drive.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}

void display_google_drive_missing_file(QWidget *p)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The app could not find the backup files on Google Drive.");
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->show();
}
} // namespace cmb
