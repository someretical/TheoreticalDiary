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

#include <ConsoleAppender.h>
#include <FileAppender.h>
#include <Logger.h>
#include <QApplication>
#include <QDir>

#include "MainWindow.h"
#include "src/core/Icons.h"
#include "src/core/RunGuard.h"
#include "styling/StyleManager.h"
#include "util/Util.h"

const char *FORMAT_STRING = "[%{Type:-7}] <%{line:4-:%{Function}> %{message}\n";

void setupApp()
{
    QFile file(":/meta/version");
    file.open(QIODevice::ReadOnly);
    QApplication::setApplicationVersion(file.readAll());

    QApplication::setApplicationName("Theoretical Diary");
    QApplication::setDesktopFileName(":/meta/me.someretical.TheoreticalDiary.desktop");
    QApplication::setWindowIcon(QIcon(":/icons/apps/theoreticaldiary.svg"));
}

void setupLogging()
{
    auto path = util::dataPath();
    auto log = path + "/log";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");

    QFile::remove(log);

    auto fileAppender = new FileAppender(log);
    fileAppender->setFormat(FORMAT_STRING);
    auto consoleAppender = new ConsoleAppender;
    consoleAppender->setFormat(FORMAT_STRING);

    cuteLogger->registerAppender(fileAppender);
    cuteLogger->registerAppender(consoleAppender);
}

auto main(int argc, char *argv[]) -> int
{
#ifdef QT_DEBUG
    RunGuard guard("Theoretical Diary Debug");
#else
    RunGuard guard("Theoretical Diary");
#endif
    if (!guard.tryToRun())
        return 0;

    QApplication app(argc, argv);

    setupLogging();
    setupApp();

    StyleManager sm{};
    Icons icns{};

    qDebug() << QIcon::themeSearchPaths();

    MainWindow w{};
    w.show();

    return QApplication::exec();
}
