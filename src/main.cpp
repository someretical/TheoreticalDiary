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

#include "core/theoreticaldiary.h"
#include "gui/mainwindow.h"
#include "util/runguard.h"

#include <QtCore>

int main(int argc, char **argv) {
  // Make sure only 1 instance of the app is running at all times
  // Courtesy of https://stackoverflow.com/a/28172162
  RunGuard guard("theoreticaldiary");
  if (!guard.tryToRun())
    return 0;

  QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  // Remove ? button in the title bar (only on Windows)
  QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

  // Remove icons from context menus
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

  // This is required so std::string can be passed via signals and slots.
  qRegisterMetaType<std::string>();

  TheoreticalDiary app(argc, argv);
  MainWindow window;

  QObject::connect(&app, &TheoreticalDiary::applicationStateChanged, &window,
                   &MainWindow::focus_changed, Qt::QueuedConnection);
  window.show();

  return app.exec();
}
