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

#ifndef CUSTOMMESSAGEBOXES_H
#define CUSTOMMESSAGEBOXES_H

#include "../core/theoreticaldiary.h"

#include <QtWidgets>
#include <string>

namespace td {
int ok_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom);
int yn_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom);
int ync_messagebox(QWidget *parent, std::string const &&accept_text, std::string const &&reject_text,
    std::string const &&destroy_text, std::string &&top, std::string const &&bottom);
} // namespace td

#endif // CUSTOMMESSAGEBOXES_H
