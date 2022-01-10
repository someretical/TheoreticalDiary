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

long unsigned int const MAX_LINE_LENGTH = 100;

namespace td {
int ok_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom)
{
    misc::clear_message_boxes();

    QMessageBox msgbox(parent);
    QPushButton ok_button("OK", &msgbox);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    msgbox.setFont(f);
    misc::extend_top_line(top, MAX_LINE_LENGTH);
    msgbox.setText(top.data());
    msgbox.setInformativeText(bottom.data());
    msgbox.addButton(&ok_button, QMessageBox::AcceptRole);
    msgbox.setDefaultButton(&ok_button);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}

int yn_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom)
{
    misc::clear_message_boxes();

    QMessageBox msgbox(parent);

    QPushButton yes("YES", &msgbox);
    QFont f = yes.font();
    f.setPointSize(11);
    yes.setFont(f);
    yes.setStyleSheet(misc::get_danger_stylesheet());
    QPushButton no("NO", &msgbox);
    no.setFlat(true);
    no.setFont(f);

    msgbox.setFont(f);
    misc::extend_top_line(top, MAX_LINE_LENGTH);
    msgbox.setText(top.data());
    msgbox.setInformativeText(bottom.data());
    msgbox.addButton(&yes, QMessageBox::AcceptRole);
    msgbox.addButton(&no, QMessageBox::RejectRole);
    msgbox.setDefaultButton(&no);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}

int ync_messagebox(QWidget *parent, std::string const &&accept_text, std::string const &&reject_text,
    std::string const &&destroy_text, std::string &&top, std::string const &&bottom)
{
    misc::clear_message_boxes();

    QMessageBox msgbox(parent);

    QPushButton destroy_button(destroy_text.data(), &msgbox);
    QFont f = destroy_button.font();
    f.setPointSize(11);
    destroy_button.setFont(f);
    destroy_button.setStyleSheet(misc::get_danger_stylesheet());

    QPushButton accept_button(accept_text.data(), &msgbox);
    accept_button.setFont(f);
    QPushButton cancel_button(reject_text.data(), &msgbox);
    cancel_button.setFlat(true);
    cancel_button.setFont(f);
    msgbox.setFont(f);
    misc::extend_top_line(top, MAX_LINE_LENGTH);
    msgbox.setText(top.data());
    msgbox.setInformativeText(bottom.data());
    msgbox.addButton(&accept_button, QMessageBox::AcceptRole);
    msgbox.addButton(&cancel_button, QMessageBox::RejectRole);
    msgbox.addButton(&destroy_button, QMessageBox::DestructiveRole);
    msgbox.setDefaultButton(&accept_button);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}
} // namespace td

namespace cmb {
bool prompt_confirm_overwrite(QWidget *p)
{
    struct stat buf;
    auto const &path = InternalManager::instance()->data_location().toStdString() + "/diary.dat";

    // Check if file exists https://stackoverflow.com/a/6296808
    if (stat(path.c_str(), &buf) != 0)
        return true;

    auto res = td::yn_messagebox(p, "Existing diary found.", "Are you sure you want to overwrite the existing diary?");

    return QMessageBox::AcceptRole == res;
}

void display_auth_error(QWidget *p)
{
    td::ok_messagebox(p, "Authentication error.", "The app was unable to authenticate with Google.");
}

void display_network_error(QWidget *p)
{
    td::ok_messagebox(p, "Network error.", "The app encountered a network error.");
}

void display_io_error(QWidget *p)
{
    td::ok_messagebox(p, "IO error.", "The app was unable to read from/write to the specified location.");
}

void display_drive_file_error(QWidget *p)
{
    td::ok_messagebox(
        p, "Google Drive error.", "The app was unable to write to/download the specified Google Drive files.");
}

void save_error(QWidget *p)
{
    td::ok_messagebox(p, "Save error.", "The app was unable to save the contents of the diary.");
}

void dev_unknown_file(QWidget *p)
{
    td::ok_messagebox(p, "Access failed.", "The app could not access the specified location.");
}

int confirm_exit_to_main_menu(QWidget *p)
{
    return td::ync_messagebox(p, "Save", "Cancel", "Do not save", "There are unsaved changes.",
        "Are you sure you want to quit without saving?");
}

void diary_downloaded(QWidget *p)
{
    td::ok_messagebox(p, "Diary downloaded.", "The diary has been downloaded from Google Drive.");
}

void diary_uploaded(QWidget *p)
{
    td::ok_messagebox(p, "Diary uploaded.", "The diary has been uploaded to Google Drive.");
}
} // namespace cmb
