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
bool is_not_space(int ch);

// Inline functions MUST be defined in header files if they are to be used in other files!
inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));
    return s;
}

inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
    return s;
}

inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}
} // namespace misc

#endif // MISC_H
