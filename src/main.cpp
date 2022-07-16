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

#include "MainWindow.h"
#include "core/theoreticaldiary.h"
#include "styling/StyleManager.h"
#include "util/RunGuard.h"
#include "util/Util.h"

const char *FORMAT_STRING = "[%{Type:-7}] <%{line:4-:%{Function}> %{message}\n";

void setup_app()
{
    QFile file(QStringLiteral(":/meta/version"));
    file.open(QIODevice::ReadOnly);
    QApplication::setApplicationVersion(file.readAll());

    QApplication::setApplicationName(QStringLiteral("Theoretical Diary"));
    QApplication::setDesktopFileName(QStringLiteral(":/meta/me.someretical.TheoreticalDiary.desktop"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/theoreticaldiary.svg")));
}

void setup_logging()
{
    auto path = util::data_path();
    auto log = path + "/log";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");

    QFile::remove(log);

    auto file_appender = new FileAppender(log);
    file_appender->setFormat(FORMAT_STRING);
    auto console_appender = new ConsoleAppender;
    console_appender->setFormat(FORMAT_STRING);

    cuteLogger->registerAppender(file_appender);
    cuteLogger->registerAppender(console_appender);
}

int main(int argc, char *argv[])
{
    RunGuard guard(QStringLiteral("Theoretical Diary"));
    if (!guard.try_to_run())
        return 0;

    QApplication app(argc, argv);

    setup_logging();
    setup_app();

    StyleManager sm;

    MainWindow w;
    w.show();

    return QApplication::exec();
}
