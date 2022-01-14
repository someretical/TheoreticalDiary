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

#include "../core/internalmanager.h"
#include "misc.h"

#include <QtWidgets>
#include <string>

namespace td {
void ok_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom);
int yn_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom);
int ync_messagebox(QWidget *parent, std::string const &&accept_text, std::string const &&reject_text,
    std::string const &&destroy_text, std::string &&top, std::string const &&bottom);
} // namespace td

namespace cmb {
bool prompt_confirm_overwrite(QWidget *parent);
void display_scope_mismatch(QWidget *p);
void display_auth_error(QWidget *parent);
void display_io_error(QWidget *parent);
void display_drive_file_error(QWidget *parent);
void save_error(QWidget *parent);
void dev_unknown_file(QWidget *p);
int confirm_exit_to_main_menu(QWidget *p);
void diary_downloaded(QWidget *p);
void diary_uploaded(QWidget *p);
} // namespace cmb

#endif // CUSTOMMESSAGEBOXES_H
