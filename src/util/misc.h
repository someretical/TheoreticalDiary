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

#ifndef MISC_H
#define MISC_H

#include <QtWidgets>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace misc {
QString get_day_suffix(int const day);
std::string get_trunc_first_line(std::string input, int const max_line_len);
void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
} // namespace misc

#endif // MISC_H
