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

#include "theoreticaldiary.h"

#include <QDir>
#include <QFontDatabase>
#include <QIcon>
#include <QStandardPaths>
#include <fstream>

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[])
    : QApplication(argc, argv) {
  gwrapper = new GoogleWrapper(this);
  diary_holder = new DiaryHolder();
  encryptor = new Encryptor();
  local_settings = new td::LocalSettings{"", "", false};
  unsaved_changes = new bool(false);

  // Create app directory
  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
           "/TheoreticalDiary");
  if (!dir.exists())
    dir.mkpath(".");

  // Load fonts
  QFontDatabase::addApplicationFont(":/Roboto-Black.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-BlackItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Bold.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-BoldItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Italic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Light.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-LightItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Medium.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-MediumItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Regular.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Thin.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-ThinItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-Bold.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-BoldItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-Italic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-Light.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-LightItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto-Condensed-Regular.ttf");

  setOrganizationName("someretical");
  setOrganizationDomain("someretical.github.io");
  setApplicationName("Theoretical Diary");
  setWindowIcon(QIcon(":/icons/hicolor/256/apps/theoreticaldiary.png"));
}

TheoreticalDiary::~TheoreticalDiary() {
  delete gwrapper;
  delete diary_holder;
  delete encryptor;
  delete local_settings;
  delete unsaved_changes;

  worker_thread.quit();
  worker_thread.wait();
}

// static specifier is not needed here (if it was, it would cause a compiler
// error) see https://stackoverflow.com/a/31305772
TheoreticalDiary *TheoreticalDiary::instance() {
  return static_cast<TheoreticalDiary *>(QApplication::instance());
}

void TheoreticalDiary::changes_made() { *unsaved_changes = true; }

void TheoreticalDiary::load_settings() {
  std::ifstream ifs(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/settings.json");
  if (ifs.fail())
    return;

  std::string content;
  ifs.seekg(0, std::ios::end);
  content.resize(ifs.tellg());
  ifs.seekg(0, std::ios::beg);
  ifs.read(content.data(), content.size());
  ifs.close();

  auto json = nlohmann::json::parse(content, nullptr, false);
  if (json.is_discarded())
    return;

  *local_settings = json.get<td::LocalSettings>();
}

bool TheoreticalDiary::save_settings() {
  nlohmann::json json = *local_settings;

  std::ofstream ofs(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/settings.json");

  if (ofs.fail())
    return false;

  ofs << json.dump();
  ofs.close();

  return true;
}
