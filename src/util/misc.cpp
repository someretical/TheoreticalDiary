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

#include "misc.h"

namespace misc {
QString get_day_suffix(int const day)
{
    switch (day) {
    case 1:
    case 21:
    case 31:
        return "st";
    case 2:
    case 22:
        return "nd";
    case 3:
    case 23:
        return "rd";
    default:
        return "th";
    }
}

bool is_not_space(int ch)
{
    return !std::isspace(ch);
}

// Assumes trimmed input!!!
std::string get_trunc_first_line(std::string input, int max_line_len)
{
    std::string first_line;
    auto newlines_exist = input.find('\n') != std::string::npos;

    if (newlines_exist) {
        std::istringstream stream(input);
        std::getline(stream, first_line);
    }
    else {
        first_line = std::move(input);
    }

    // Truncate line and append ... if it's too long.
    // Cast max_line_len from 'int const' to 'long unsigned int'.
    if (first_line.size() > static_cast<std::make_unsigned<decltype(max_line_len)>::type>(max_line_len)) {
        first_line.resize(max_line_len);
        first_line.append("...");
    }
    else if (newlines_exist) {
        // There can be multiple lines in an entry where the first line does not have at least LONGEST_LINE_LENGTH
        // characters.
        first_line.append("...");
    }

    return first_line;
}
} // namespace misc
