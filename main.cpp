/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "runguard.h"
#include "theoreticaldiary.h"

#include <QApplication>
#include <QFontDatabase>
#include <QStandardPaths>

int main(int argc, char **argv) {
  // Make sure only 1 instance of the app is running at all times
  // Courtesy of https://stackoverflow.com/a/28172162
  RunGuard guard("theoretical-diary");
  if (!guard.tryToRun())
    return 0;

  // This is to fix fonts not scaling properly at different DPI
  // https://stackoverflow.com/a/36058882
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  // Remove ? button in the title bar (only on Windows)
  QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

  QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
  if (!dir.exists())
    dir.mkpath(".");

  TheoreticalDiary a(argc, argv);
  MainWindow w;

  // Windows does not have Ubuntu by default
  QFontDatabase fdb;
  fdb.addApplicationFont(":/fonts/Ubuntu-Regular.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-MediumItalic.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-Medium.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-LightItalic.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-Light.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-Italic.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-BoldItalic.ttf");
  fdb.addApplicationFont(":/fonts/Ubuntu-Bold.ttf");

  w.show();
  return a.exec();
}
