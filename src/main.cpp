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

#include <QtCore>
#include <string>

#include "core/internalmanager.h"
#include "core/theoreticaldiary.h"
#include "gui/mainwindow.h"
#include "o2requestor.h"
#include "util/runguard.h"

Q_DECLARE_METATYPE(td::NR)
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(td::CalendarButtonData)

int main(int argc, char **argv)
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

    // Make sure only 1 instance of the app is running at all times. Courtesy of https://stackoverflow.com/a/28172162
    RunGuard guard("theoreticaldiary");
    if (!guard.try_to_run()) {
        qDebug() << "Quitting as an instance of this app already exists.";
        return 0;
    }

    QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Remove ? button in the title bar (only on Windows).
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

    // Remove icons from context menus
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // Register types so they can be used in signals and slots.
    qRegisterMetaType<std::string>();
    qRegisterMetaType<td::CalendarButtonData>();
    qRegisterMetaType<td::NR>();

    TheoreticalDiary app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
