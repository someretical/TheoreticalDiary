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

#include "theoreticaldiary.h"
#include "../util/encryptor.h"
#include "diaryholder.h"
#include "googlewrapper.h"
#include "internalmanager.h"

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[]) : QApplication(argc, argv)
{
    // Set app version.
    QFile file(":/VERSION.txt");
    file.open(QIODevice::ReadOnly);
    setApplicationVersion(file.readAll());

    setApplicationName("Theoretical Diary");

    // o2 needs these 2 set apparently.
    setOrganizationName("someretical");
    setOrganizationDomain("someretical.com");

    setDesktopFileName("me.someretical.TheoreticalDiary.desktop");
    setWindowIcon(QIcon(":/linux_icons/256x256/theoreticaldiary.png"));

    // Create app directory.
    QDir dir(internal_manager->data_location());
    if (!dir.exists())
        dir.mkpath(".");

    gwrapper = new GoogleWrapper(this);
    diary_holder = new DiaryHolder();
    encryptor = new Encryptor();
    internal_manager = new InternalManager();
}

TheoreticalDiary::~TheoreticalDiary()
{
    delete gwrapper;
    delete diary_holder;
    delete encryptor;
    delete internal_manager;
}

TheoreticalDiary *TheoreticalDiary::instance()
{
    return static_cast<TheoreticalDiary *>(QApplication::instance());
}
