/**
 * This file is part of Theoretical Diary.
 *
 * Theoretical Diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Theoretical Diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "runguard.h"
#include "theoreticaldiary.h"

#include <QApplication>

int main(int argc, char **argv) {
  // Make sure only 1 instance of the app is running at all times
  // Courtesy of https://stackoverflow.com/a/28172162
  RunGuard guard("Theoretical Diary");
  if (!guard.tryToRun())
    return 0;

  // This is to fix fonts not scaling properly at different DPI
  // https://stackoverflow.com/a/36058882
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  // Remove ? button in the title bar (only on Windows)
  QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

  TheoreticalDiary a(argc, argv);

  MainWindow w;
  w.show();

  return a.exec();
}
