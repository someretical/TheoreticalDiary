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

#include <fstream>

#include "../util/encryptor.h"
#include "../util/zipper.h"
#include "diaryholder.h"

int const CURRENT_SAVE_VERSION = 5;
DiaryHolder *diary_holder_ptr;

DiaryHolder::DiaryHolder()
{
    diary_holder_ptr = this;
    m_diary = td::Diary{td::DiaryLog(), td::Metadata{CURRENT_SAVE_VERSION, QDateTime::currentSecsSinceEpoch()}};
}

DiaryHolder::~DiaryHolder() {}

DiaryHolder *DiaryHolder::instance()
{
    return diary_holder_ptr;
}

void DiaryHolder::init()
{
    qDebug() << "Initialised diary.";
    m_diary = td::Diary{td::DiaryLog(), td::Metadata{CURRENT_SAVE_VERSION, QDateTime::currentSecsSinceEpoch()}};
}

bool DiaryHolder::load(std::string &raw)
{
    auto const &json = nlohmann::json::parse(raw, nullptr, false);
    if (json.is_discarded()) {
        qDebug() << "Invalid diary JSON being loaded.";
        return false;
    }

    try {
        m_diary = json.get<td::Diary>();
    }
    catch (nlohmann::json::exception const &e) {
        qDebug() << "Exception while loading JSON diary.";
        return false;
    }

    qDebug() << "JSON diary loading successful.";
    return true;
}

bool DiaryHolder::save()
{
    std::string primary_path = InternalManager::instance()->data_location().toStdString() + "/diary.dat";
    std::string backup_path = InternalManager::instance()->data_location().toStdString() + "/diary.dat.bak";

    // Backup existing diary first.
    std::ifstream src(primary_path, std::ios::binary);
    if (!src.fail()) {
        std::ofstream dst(backup_path, std::ios::binary);

        if (dst.fail()) {
            qDebug() << "Couldn't find secondary backup path for saving diary.";
            return false;
        }

        dst << src.rdbuf();
    }

    // Update last_updated.
    m_diary.metadata.last_updated = QDateTime::currentSecsSinceEpoch();
    nlohmann::json const j = m_diary;

    // Gzip JSON.
    std::string compressed, encrypted, decompressed = j.dump();
    Zipper::zip(compressed, decompressed);

    // Encrypt if there is a password set.
    auto const key_set = Encryptor::instance()->m_key_set;
    if (key_set)
        Encryptor::instance()->encrypt(compressed, encrypted);

    // Write to file.
    std::ofstream ofs(primary_path, std::ios::binary);
    if (!ofs.fail()) {
        ofs << (key_set ? encrypted : compressed);

        InternalManager::instance()->m_internal_diary_changed = false;
        InternalManager::instance()->m_diary_file_changed = true;
        qDebug() << "Saved diary to disk.";
        return true;
    }
    else {
        qDebug() << "Couldn't write to diary location on disk.";
        return false;
    }
}

std::optional<td::DiaryLog::iterator> DiaryHolder::get_yearmap(QDate const &date)
{
    auto const &year_iter = m_diary.log.find(date.year());
    if (year_iter == m_diary.log.end())
        return std::nullopt;

    return std::optional(year_iter);
}

std::optional<td::YearMap::iterator> DiaryHolder::get_monthmap(QDate const &date)
{
    auto const &year_iter = m_diary.log.find(date.year());
    if (year_iter == m_diary.log.end())
        return std::nullopt;

    auto const &month_iter = year_iter->second.find(date.month());
    if (month_iter == year_iter->second.end())
        return std::nullopt;

    return std::optional(month_iter);
}

std::optional<td::MonthMap::iterator> DiaryHolder::get_entry(QDate const &date)
{
    auto const &year_iter = m_diary.log.find(date.year());
    if (year_iter == m_diary.log.end())
        return std::nullopt;

    auto const &month_iter = year_iter->second.find(date.month());
    if (month_iter == year_iter->second.end())
        return std::nullopt;

    auto const &entry_iter = month_iter->second.find(date.day());
    if (entry_iter == month_iter->second.end())
        return std::nullopt;

    return std::optional(entry_iter);
}

void DiaryHolder::create_entry(QDate const &date, td::Entry const &entry)
{
    auto &year_map = DiaryHolder::instance()->m_diary.log;
    auto const &[year_iter, dummy] = year_map.try_emplace(date.year(), td::YearMap());
    auto const &[month_iter, d2] = year_iter->second.try_emplace(date.month(), td::MonthMap());

    month_iter->second[date.day()] = entry;
}

void DiaryHolder::delete_entry(QDate const &date)
{
    auto const &opt = get_monthmap(date);
    if (!opt)
        return;

    (*opt)->second.erase(date.day());
}
