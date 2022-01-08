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

std::string get_trunc_first_line(std::string const &input, int const max_line_len)
{
    // Get first line https://stackoverflow.com/a/5059112
    std::istringstream f(input);
    std::string second_line_exists, res;
    std::getline(f, res);

    // Truncate line and append ... if it's too long.
    if (max_line_len < res.size()) {
        res.resize(max_line_len);

        res.append("...");
    }
    else if (std::getline(f, second_line_exists)) {
        // There can be multiple lines in an entry where the first line does not have at least LONGEST_LINE_LENGTH
        // characters.
        res.append("...");
    }
}

} // namespace misc
