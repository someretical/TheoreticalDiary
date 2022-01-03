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

#include "diaryholder.h"
#include "theoreticaldiary.h"

int const CURRENT_SAVE_VERSION = 4;

DiaryHolder::DiaryHolder()
{
    diary = new td::Diary;
}

DiaryHolder::~DiaryHolder()
{
    delete diary;
}

void DiaryHolder::init()
{
    *diary = td::Diary{
        td::DiaryLog(), td::Metadata{CURRENT_SAVE_VERSION, QDateTime::currentSecsSinceEpoch()}, td::Settings{false}};
}

bool DiaryHolder::load(std::string &raw)
{
    auto const &json = nlohmann::json::parse(raw, nullptr, false);
    if (json.is_discarded())
        return false;

    // Here is where updating save versions will occur *if* I every actually get
    // around to doing this which will probably never happen because I'm the only
    // person using my own app :')

    try {
        *diary = json.get<td::Diary>();
    }
    catch (nlohmann::json::exception const &e) {
        return false;
    }

    return true;
}

std::optional<td::DiaryLog::iterator> DiaryHolder::get_yearmap(QDate const &date) const
{
    auto const &iter = diary->log.find(date.year());
    if (iter == diary->log.end())
        return std::nullopt;

    return std::optional(iter);
}

std::optional<td::YearMap::iterator> DiaryHolder::get_monthmap(QDate const &date) const
{
    auto const &opt = get_yearmap(date);
    if (!opt)
        return std::nullopt;

    auto &[dummy, yearmap] = **opt;
    auto const &iter = yearmap.find(date.month());
    if (iter == yearmap.end())
        return std::nullopt;

    return std::optional(iter);
}

std::optional<td::MonthMap::iterator> DiaryHolder::get_entry(QDate const &date) const
{
    auto const &opt = get_monthmap(date);
    if (!opt)
        return std::nullopt;

    auto &[dummy, monthmap] = **opt;
    auto const &iter = monthmap.find(date.day());
    if (iter == monthmap.end())
        return std::nullopt;

    return std::optional(iter);
}

void DiaryHolder::create_entry(QDate const &date, td::Entry const &entry)
{
    // Find/create path to new entry
    // https://stackoverflow.com/a/101980

    // Find/create YearMap
    auto const &log_map = &TheoreticalDiary::instance()->diary_holder->diary->log;
    auto log_iter = log_map->lower_bound(date.year());

    if (log_iter == log_map->end() || log_map->key_comp()(date.year(), log_iter->first))
        log_iter = log_map->insert(log_iter, td::DiaryLog::value_type(date.year(), td::YearMap()));

    // Find/create MonthMap
    auto const &year_map = &log_iter->second;
    auto year_iter = year_map->lower_bound(date.month());

    if (year_iter == year_map->end() || year_map->key_comp()(date.month(), year_iter->first))
        year_iter = year_map->insert(year_iter, td::YearMap::value_type(date.month(), td::MonthMap()));

    // Find/create Entry
    auto const &month_map = &year_iter->second;
    auto month_iter = month_map->lower_bound(date.day());

    if (month_iter == month_map->end() || month_map->key_comp()(date.day(), month_iter->first)) {
        month_iter = month_map->insert(month_iter, td::MonthMap::value_type(date.day(), entry));
    }
    else {
        month_iter->second = entry;
    }
}

void DiaryHolder::delete_entry(QDate const &date)
{
    auto const &opt = get_monthmap(date);
    if (!opt)
        return;

    (*opt)->second.erase(date.day());
}
