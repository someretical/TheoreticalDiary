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

#include <sstream>

#include "../core/internalmanager.h"
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

void extend_top_line(std::string &top, long unsigned int const max_line_len)
{
    if (top.size() < max_line_len)
        top.append(max_line_len - top.size(), ' ');
}

void clear_message_boxes()
{
    QWidget *w;
    while ((w = QApplication::activeModalWidget())) {
        auto dialog = qobject_cast<QDialog *>(w);

        if (!dialog) {
            qDebug() << "Failed to cast widget pointer into dialog pointer:" << w;
        }
        else {
            qDebug() << "Removing dialog:" << w;
            dialog->done(QMessageBox::RejectRole);
        }
    }
}

QColor rating_to_colour(const td::Rating rating)
{
    auto intman = InternalManager::instance();

    switch (rating) {
    case td::Rating::Unknown:
        return intman->colour(td::ColourRole::Unknown);
    case td::Rating::VeryBad:
        return intman->colour(td::ColourRole::VeryBad);
    case td::Rating::Bad:
        return intman->colour(td::ColourRole::Bad);
    case td::Rating::Ok:
        return intman->colour(td::ColourRole::Ok);
    case td::Rating::Good:
        return intman->colour(td::ColourRole::Good);
    case td::Rating::VeryGood:
        return intman->colour(td::ColourRole::VeryGood);
    }

    // This can never happen, it's only here to shut down the compiler warning.
    return intman->colour(td::ColourRole::Unknown);
}

td::Theme rating_to_theme(const td::Rating rating)
{
    switch (rating) {
    case td::Rating::Unknown:
        if (InternalManager::instance()->get_theme() == td::Theme::Light)
            return td::Theme::Dark;
        else
            return td::Theme::Light;

    case td::Rating::VeryBad:
    case td::Rating::Bad:
    case td::Rating::Ok:
        return td::Theme::Light;
    case td::Rating::Good:
    case td::Rating::VeryGood:
        return td::Theme::Dark;
    }

    // This can never happen, it's only here to shut down the compiler warning.
    return td::Theme::Dark;
}

static const auto light_text = QColor("#CACBCE");
static const auto dark_text = QColor("#1D1D20");

QColor theme_to_text(const td::Theme theme)
{
    if (td::Theme::Light == theme) {
        return light_text;
    }
    else {
        return dark_text;
    }
}

QString sanitise_html(std::string const &str)
{
    return QString::fromStdString(str)
        .replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace("\n", "<br>");
}

} // namespace misc
