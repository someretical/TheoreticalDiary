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

#ifndef THOERETICALDIARY_H
#define THOERETICALDIARY_H

class DiaryHolder;
class Encryptor;
class GoogleWrapper;
class InternalManager;

#include <QtWidgets>
#include <string>
#include <sys/stat.h>

/*
 * TheoreticalDiary class
 */
class TheoreticalDiary : public QApplication {
    Q_OBJECT

public:
    TheoreticalDiary(int &argc, char **argv);
    ~TheoreticalDiary();
    static TheoreticalDiary *instance();

    // These have to be pointers in order to break cyclic dependencies.
    GoogleWrapper *gwrapper;
    QSettings *settings;
    DiaryHolder *diary_holder;
    Encryptor *encryptor;
    InternalManager *internal_manager;
};

#endif // THOERETICALDIARY_H
