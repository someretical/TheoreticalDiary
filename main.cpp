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

int main(int argc, char **argv) {
  // This is to fix fonts not scaling properly at different DPI
  // https://stackoverflow.com/a/36058882
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  // Make sure only 1 instance of the app is running at all times
  // Courtesy of https://stackoverflow.com/a/28172162
  RunGuard guard("theoretical-diary");
  if (!guard.tryToRun())
    return 0;

  TheoreticalDiary app(argc, argv);
  MainWindow mainwindow;

  mainwindow.show();
  return app.exec();
}
