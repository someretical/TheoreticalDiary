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

#ifndef DIARYHOLDER_H
#define DIARYHOLDER_H

#include <QtWidgets>
#include <optional>
#include <string>

#include "internalmanager.h"

class DiaryHolder {
public:
    DiaryHolder();
    ~DiaryHolder();
    static DiaryHolder *instance();

    bool load(std::string &raw);
    bool load(std::string &&raw)
    {
        // If there's ever going to be more than one argument, see
        // https://stackoverflow.com/questions/17644133/function-that-accepts-both-lvalue-and-rvalue-arguments#comment25693290_17644133
        return load(raw);
    }
    bool save(); // It is the responsibility of the caller to start and end busy mode as well as show the error!
    void init();

    std::optional<td::DiaryLog::iterator> get_yearmap(QDate const &date);
    std::optional<td::YearMap::iterator> get_monthmap(QDate const &date);
    std::optional<td::MonthMap::iterator> get_entry(QDate const &date);
    void create_entry(QDate const &date, td::Entry const &entry);
    void delete_entry(QDate const &date);

    td::Diary m_diary;
};

#endif // DIARYHOLDER_H
