/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THEORETICAL_DIARY_DIARY_H
#define THEORETICAL_DIARY_DIARY_H

#include "TimeDelta.h"

#include <QDate>
#include <QHash>
#include <QString>
#include <QVector>

namespace DiaryRating {
enum Rating { Unknown, VeryBad, Bad, Ok, Good, VeryGood };
}

struct Quote {
    QDateTime lastUpdated;
    int32_t id;
    QString quote;
    QString author;
};

struct Note {
    QDateTime lastUpdated;
    int32_t id;
    QString name;
    QString contents;
};

struct Reminder {
    QDateTime lastUpdated;
    int32_t id;
    QDate baseDate;
    TimeDelta delta;
    int32_t repeatTimes; // 0 means repeat indefinitely
    QString text;
};

struct ReminderCheck {
    QDateTime lastUpdated;
    int32_t parentID;
    bool isComplete;
    QString note;
};

struct Entry {
    QDateTime lastUpdated;
    bool important;
    DiaryRating::Rating rating;
    QString generalMessage;
    QString emotionsMessage;
    QString gratitudeMessage;
    QVector<QString> gratitudeList;
    QVector<ReminderCheck> reminderChecks;
};

struct Year {
    QDateTime lastUpdated;
    QDate date;
    QHash<QDate, Entry> entries;
};

struct EntryPreviewData {
    bool important = false;
    DiaryRating::Rating rating = DiaryRating::Unknown;
};

class Diary {
public:
    Diary();
    ~Diary();

    QDateTime lastUpdated;
    QHash<int, Year> years;
    QVector<Reminder> reminders;
    QVector<Quote> quotes;
    QVector<Note> notes;
};

#endif // THEORETICAL_DIARY_DIARY_H
