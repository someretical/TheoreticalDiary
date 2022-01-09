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

#include "theoreticaldiary.h"

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[]) : QApplication(argc, argv)
{
    // Load main font.
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Black.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-BlackItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Italic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-MediumItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-ThinItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Bold.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Italic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Light.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Regular.ttf");

    // Load monospace font.
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Thin.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-ExtraLight.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Regular.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Medium.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Bold.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-ThinItalic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-ExtraLightItalic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Italic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-MediumItalic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-SemiBoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-BoldItalic.ttf");

    // Set app version.
    QFile file(":/VERSION.txt");
    file.open(QIODevice::ReadOnly);
    setApplicationVersion(file.readAll());
    file.close();

    setApplicationName("Theoretical Diary");

    // o2 needs these 2 set apparently.
    setOrganizationName("someretical");
    setOrganizationDomain("someretical.com");

    setDesktopFileName("me.someretical.TheoreticalDiary.desktop");
    setWindowIcon(QIcon(":/linux_icons/256x256/theoreticaldiary.png"));

    gwrapper = new GoogleWrapper(this);
    diary_holder = new DiaryHolder();
    encryptor = new Encryptor();
    internal_manager = new InternalManager();

    // Create app directory.
    QDir dir(internal_manager->data_location());
    if (!dir.exists())
        dir.mkpath(".");
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
