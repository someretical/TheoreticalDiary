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

#ifndef CUSTOMMESSAGEBOXES_H
#define CUSTOMMESSAGEBOXES_H

#include <QtWidgets>
#include <sys/stat.h>

#include "../core/internalmanager.h"
#include "misc.h"

namespace cmb {
void display_local_io_error(QWidget *p);
void display_local_diary_save_error(QWidget *p);
void display_google_drive_network_error(QWidget *p, QString const &&err);
void diary_uploaded(QWidget *p);
void diary_downloaded(QWidget *p);
void display_local_diary_access_error(QWidget *p);
void display_google_drive_scope_mismatch(QWidget *p);
void display_google_drive_auth_error(QWidget *p);
void display_google_drive_missing_file(QWidget *p);

template <typename Lambda> void prompt_diary_overwrite(QWidget *p, Lambda const &cb)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    struct stat buf;
    auto const &path = InternalManager::instance()->data_location().toStdString() + "/diary.dat";

    // Check if file exists https://stackoverflow.com/a/6296808
    if (stat(path.c_str(), &buf) != 0) {
        cb(QMessageBox::Yes);
        return;
    }

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("A local diary already exists!");
    msgbox->setInformativeText("Are you sure you want to overwrite it?");
    msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox->setDefaultButton(QMessageBox::No);
    QObject::connect(msgbox, &QMessageBox::finished, cb);
    msgbox->show();
}

template <typename Lambda> void confirm_switch_entries(QWidget *p, Lambda const &cb)
{
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);

    auto msgbox = new QMessageBox(p);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    msgbox->setText("The current entry has unsaved changes!");
    msgbox->setInformativeText("Would you like to save them?");
    msgbox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgbox->setDefaultButton(QMessageBox::Save);
    QObject::connect(msgbox, &QMessageBox::finished, cb);
    msgbox->show();
}
} // namespace cmb

#endif // CUSTOMMESSAGEBOXES_H
