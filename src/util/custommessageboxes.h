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
class CustomMessageBox : public QMessageBox {
    Q_OBJECT

public:
    CustomMessageBox(QWidget *p = nullptr) : QMessageBox(p) {}

protected:
    void closeEvent(QCloseEvent *event) override
    {
        emit done(QMessageBox::RejectRole);
        event->accept();
    }
};

template <typename Lambda>
void ok_messagebox(
    QWidget *p, Lambda const &cb, QString const &&top, QString const &&bottom, QString const &&ok_text = "OK")
{
    auto msgbox = new CustomMessageBox(p);
    auto ok_button = new QPushButton(ok_text, msgbox);

    msgbox->setText(top);
    msgbox->setInformativeText(bottom);
    msgbox->addButton(ok_button, QMessageBox::AcceptRole);
    msgbox->setDefaultButton(ok_button);
    msgbox->setEscapeButton(ok_button);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);

    QObject::connect(msgbox, &QMessageBox::finished, cb);
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    msgbox->show();
}

template <typename Lambda>
void yn_messagebox(QWidget *p, Lambda const &cb, QString const &&top, QString const &&bottom,
    QString const &&yes_text = "Yes", QString const &&no_text = "No",
    QMessageBox::ButtonRole const default_role = QMessageBox::RejectRole)
{
    auto msgbox = new CustomMessageBox(p);
    auto yes_button = new QPushButton(yes_text, msgbox);
    auto no_button = new QPushButton(no_text, msgbox);

    msgbox->setText(top);
    msgbox->setInformativeText(bottom);
    msgbox->addButton(yes_button, QMessageBox::AcceptRole);
    msgbox->addButton(no_button, QMessageBox::RejectRole);
    msgbox->setDefaultButton(QMessageBox::RejectRole == default_role ? no_button : yes_button);
    msgbox->setEscapeButton(QMessageBox::RejectRole == default_role ? no_button : yes_button);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);

    QObject::connect(msgbox, &QMessageBox::finished, cb);
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    msgbox->show();
}

template <typename Lambda>
void ync_messagebox(QWidget *p, Lambda const &cb, QString const &&top, QString const &&bottom,
    QString const &&yes_text = "Yes", QString const &&no_text = "No", QString const &&cancel_text = "Cancel",
    QMessageBox::ButtonRole const default_role = QMessageBox::RejectRole)
{
    auto msgbox = new CustomMessageBox(p);
    auto yes_button = new QPushButton(yes_text, msgbox);
    auto no_button = new QPushButton(no_text, msgbox);
    auto cancel_button = new QPushButton(cancel_text, msgbox);
    auto default_button = QMessageBox::RejectRole == default_role
                              ? cancel_button
                              : QMessageBox::YesRole == default_role ? yes_button : no_button;

    msgbox->setText(top);
    msgbox->setInformativeText(bottom);
    msgbox->addButton(yes_button, QMessageBox::YesRole);
    msgbox->addButton(no_button, QMessageBox::NoRole);
    msgbox->addButton(cancel_button, QMessageBox::RejectRole);
    msgbox->setDefaultButton(default_button);
    msgbox->setEscapeButton(default_button);
    msgbox->setAttribute(Qt::WA_DeleteOnClose, true);

    QObject::connect(msgbox, &QMessageBox::finished, cb);
    misc::clear_message_boxes();
    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    msgbox->show();
}

template <typename Lambda> void prompt_confirm_overwrite(QWidget *p, Lambda const &cb)
{
    struct stat buf;
    auto const &path = InternalManager::instance()->data_location().toStdString() + "/diary.dat";

    // Check if file exists https://stackoverflow.com/a/6296808
    if (stat(path.c_str(), &buf) != 0) {
        cb(QMessageBox::AcceptRole);
        return;
    }

    cmb::yn_messagebox(p, cb, "Are you sure you want to overwrite the existing diary?", "An existing diary was found.");
}

template <typename Lambda> void display_auth_error(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The app was unable to authenticate with Google.",
        "Make sure the internet is working and all required scopes have been granted.");
}

template <typename Lambda> void display_scope_mismatch(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The granted scopes do not match the minimum requirements.",
        "To fix this, press the deauthorize button and retry.");
}

template <typename Lambda> void display_io_error(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The app was unable to read from/write to the specified local location.",
        "This usually means a permission error.");
}

template <typename Lambda> void display_drive_file_error(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The app was unable to read from/write to the specified Google Drive files.", "");
}

template <typename Lambda> void save_error(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(
        p, cb, "The app was unable to save the contents of the diary.", "This usually means a permission error.");
}

template <typename Lambda> void dev_unknown_file(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(
        p, cb, "The app could not access the specified local location.", "This usually means a permission error.");
}

template <typename Lambda> void confirm_exit_to_main_menu(QWidget *p, Lambda const &cb)
{
    cmb::ync_messagebox(p, cb, "Are you sure you want to quit without saving?", "There are unsaved changes.", "Save",
        "Do not save", "Cancel");
}

template <typename Lambda> void diary_downloaded(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The diary has been downloaded from Google Drive.", "");
}

template <typename Lambda> void diary_uploaded(QWidget *p, Lambda const &cb)
{
    cmb::ok_messagebox(p, cb, "The diary has been uploaded to Google Drive.", "");
}

template <typename Lambda> void confirm_switch_date(QWidget *p, Lambda const &cb)
{
    cmb::ync_messagebox(p, cb, "Are you sure you want to switch dates without saving?", "There are unsaved changes.",
        "Save", "Do not save", "Cancel");
}
} // namespace cmb

#endif // CUSTOMMESSAGEBOXES_H
