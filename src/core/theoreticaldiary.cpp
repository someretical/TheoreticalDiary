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

#include "theoreticaldiary.h"

#include <QDir>
#include <QFontDatabase>
#include <QIcon>
#include <QStandardPaths>
#include <fstream>

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[])
    : QApplication(argc, argv) {
  // Load main font
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Black.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-BlackItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Bold.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-BoldItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Italic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Light.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-LightItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Medium.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-MediumItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Regular.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Thin.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-ThinItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Bold.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-BoldItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Italic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Light.ttf");
  QFontDatabase::addApplicationFont(
      ":/Roboto/Roboto-Condensed-LightItalic.ttf");
  QFontDatabase::addApplicationFont(":/Roboto/Roboto-Condensed-Regular.ttf");

  // Load monospace font
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Thin.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-ExtraLight.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Light.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Regular.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Medium.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-SemiBold.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Bold.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-ThinItalic.ttf");
  QFontDatabase::addApplicationFont(
      ":/RobotoMono/RobotoMono-ExtraLightItalic.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-LightItalic.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-Italic.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-MediumItalic.ttf");
  QFontDatabase::addApplicationFont(
      ":/RobotoMono/RobotoMono-SemiBoldItalic.ttf");
  QFontDatabase::addApplicationFont(":/RobotoMono/RobotoMono-BoldItalic.ttf");

  QFile file(":/VERSION.txt");
  file.open(QIODevice::ReadOnly);
  setApplicationVersion(file.readAll());
  file.close();

  setApplicationName("Theoretical Diary");
  setWindowIcon(QIcon(":/linux_icons/hicolor/256/apps/theoreticaldiary.png"));
  setDesktopFileName("io.github.someretical.theoreticaldiary.desktop");

  gwrapper = new GoogleWrapper(this);
  diary_holder = new DiaryHolder();
  encryptor = new Encryptor();
  local_settings = new td::LocalSettings{"", "", false};
  local_settings_modified = false;
  diary_modified = false;
  oauth_modified = false;
  application_theme = new QString("dark");

  // Create app directory
  QDir dir(data_location());
  if (!dir.exists())
    dir.mkpath(".");

  load_settings();
}

TheoreticalDiary::~TheoreticalDiary() {
  delete gwrapper;
  delete diary_holder;
  delete encryptor;
  delete local_settings;
  delete application_theme;

  worker_thread.quit();
  worker_thread.wait();
}

// static specifier is not needed here (if it was, it would cause a compiler
// error) see https://stackoverflow.com/a/31305772
TheoreticalDiary *TheoreticalDiary::instance() {
  return static_cast<TheoreticalDiary *>(QApplication::instance());
}

QString TheoreticalDiary::data_location() {
  return QString("%1/%2").arg(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      applicationName());
}

QString TheoreticalDiary::theme() { return *application_theme; }

void TheoreticalDiary::local_settings_changed() {
  local_settings_modified = true;
}

void TheoreticalDiary::diary_changed() { diary_modified = true; }

void TheoreticalDiary::oauth_changed() { oauth_modified = true; }

void TheoreticalDiary::load_settings() {
  std::ifstream ifs(data_location().toStdString() + "/settings.json");
  if (ifs.fail()) {
    local_settings_changed();
    return;
  }

  std::string content;
  ifs.seekg(0, std::ios::end);
  content.resize(ifs.tellg());
  ifs.seekg(0, std::ios::beg);
  ifs.read(content.data(), content.size());
  ifs.close();

  auto json = nlohmann::json::parse(content, nullptr, false);
  if (json.is_discarded()) {
    local_settings_changed();
    return;
  }

  *local_settings = json.get<td::LocalSettings>();
}

bool TheoreticalDiary::save_settings() {
  nlohmann::json json = *local_settings;
  std::ofstream ofs(data_location().toStdString() + "/settings.json");

  if (ofs.fail())
    return false;

  ofs << json.dump();
  ofs.close();

  return true;
}
