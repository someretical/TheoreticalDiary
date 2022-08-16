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

#include "core/Application.h"
#include "core/RunGuard.h"
#include "gui/MainWindow.h"

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
    Application theoreticalDiary(argc, argv);

    MainWindow w{};
    w.show();

    return theoreticalDiary.exec();
}
