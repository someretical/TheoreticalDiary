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

#ifndef THOERETICALDIARY_H
#define THOERETICALDIARY_H

class DiaryHolder;
class GoogleWrapper;

#include "../util/encryptor.h"
#include "diaryholder.h"
#include "googlewrapper.h"
#include "internalmanager.h"

#include <QtWidgets>
#include <json.hpp>
#include <string>
#include <sys/stat.h>

// This is required so std::string can be passed via signals and slots.
Q_DECLARE_METATYPE(std::string)

/*
 * TheoreticalDiary class
 */
class TheoreticalDiary : public QApplication {
    Q_OBJECT

public:
    TheoreticalDiary(int &argc, char **argv);
    ~TheoreticalDiary();
    static TheoreticalDiary *instance();

    // These two have to be pointers because the Q_OBJECT macro removes the assignment operator.
    GoogleWrapper *gwrapper;
    QSettings *settings;
    DiaryHolder *diary_holder; // Has to be a pointer due to cyclic dependency.
    Encryptor *encryptor;      // This is a pointer just for consistency's sake.
    InternalManager *internal_manager;
};

#endif // THOERETICALDIARY_H
