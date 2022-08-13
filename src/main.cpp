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

#include "core/RunGuard.h"
#include "core/Util.h"
#include "gui/Icons.h"
#include "gui/MainWindow.h"
#include "gui/styling/StyleManager.h"

const char *FORMAT_STRING = "[%{Type:-7}] <%{line:4-:%{Function}> %{message}\n";

auto main(int argc, char *argv[]) -> int
{
    // https:// stackoverflow.com/a/41701133
    // Basically allows debugging output on Windows.
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);
    }
#endif

#ifdef QT_DEBUG
    RunGuard guard("Theoretical Diary Debug");
#else
    RunGuard guard("Theoretical Diary");
#endif
    if (!guard.tryToRun())
        return 0;

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication app(argc, argv);
#ifdef QT_DEBUG
    QApplication::setApplicationVersion("DEBUG");
#else
    QFile file(":/meta/version");
    file.open(QIODevice::ReadOnly);
    QApplication::setApplicationVersion(file.readAll());
#endif
    QApplication::setApplicationName("Theoretical Diary");
    QApplication::setDesktopFileName(":/meta/me.someretical.TheoreticalDiary.desktop");
    QApplication::setWindowIcon(QIcon(QPixmap(":/icons/apps/theoreticaldiary.svg")
                                          .scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    // Remove ? button in the title bar (only on Windows).
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton, true);

#ifdef Q_OS_LINUX
    auto testFont = QFont();
    if (testFont.defaultFamily() == "DejaVu Sans") {
        auto ubuntu = QFont("Ubuntu");
        ubuntu.setPointSize(11);
        QApplication::setFont(ubuntu);
    }
#endif

#ifdef Q_OS_WIN
    // Qt on Windows uses "MS Shell Dlg 2" as the default font for many widgets, which resolves
    // to Tahoma 8pt, whereas the correct font would be "Segoe UI" 9pt.
    // Apparently, some widgets are already using the correct font. Thanks, MuseScore for this neat fix!
    QApplication::setFont(QApplication::font("QMessageBox"));
#endif

    auto path = dataPath();
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

    styleManager();
    icons();

    MainWindow w{};
    w.show();

    return QApplication::exec();
}
