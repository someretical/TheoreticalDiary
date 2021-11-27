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

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[])
    : QApplication(argc, argv) {
  load_fonts();
  worker_thread.start();

  // Set app version.
  QFile file(":/VERSION.txt");
  file.open(QIODevice::ReadOnly);
  setApplicationVersion(file.readAll());
  file.close();

  setApplicationName("Theoretical Diary");

  // o2 needs these 2 set apparently.
  setOrganizationName("someretical");
  setOrganizationDomain("someretical.com");

  setWindowIcon(QIcon(":/linux_icons/hicolor/25"
                      "6/apps/theoreticaldiary.png"));
  setDesktopFileName("io.github.someretical.theoreticaldiary.desktop");

  gwrapper = new GoogleWrapper(this);
  diary_holder = new DiaryHolder();
  encryptor = new Encryptor();
  diary_modified = false;
  closeable = true;
  application_theme = new QString("dark");

  // Load global stylesheets
  file.setFileName(QString(":/%1/dangerbutton.qss")
                       .arg(TheoreticalDiary::instance()->theme()));
  file.open(QIODevice::ReadOnly);
  danger_button_style = new QString(file.readAll());
  file.close();

  // Create app directory
  QDir dir(data_location());
  if (!dir.exists())
    dir.mkpath(".");

  settings = new QSettings(
      QString("%1/%2").arg(TheoreticalDiary::instance()->data_location(),
                           "config.ini"),
      QSettings::IniFormat, this);
  if (!settings->contains("sync_enabled"))
    settings->setValue("sync_enabled", false);
}

TheoreticalDiary::~TheoreticalDiary() {
  delete gwrapper;
  delete diary_holder;
  delete encryptor;
  delete settings;
  delete application_theme;
  delete danger_button_style;

  worker_thread.quit();
  worker_thread.wait();
}

// static specifier is not needed here (if it was, it would cause a compiler
// error) see https://stackoverflow.com/a/31305772
TheoreticalDiary *TheoreticalDiary::instance() {
  return static_cast<TheoreticalDiary *>(QApplication::instance());
}

void TheoreticalDiary::load_fonts() {
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
}

QString TheoreticalDiary::data_location() {
  return QString("%1/%2").arg(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      applicationName());
}

QString TheoreticalDiary::theme() { return *application_theme; }

void TheoreticalDiary::diary_changed() { diary_modified = true; }

bool TheoreticalDiary::confirm_overwrite(QWidget *p) {
  struct stat buf;
  const std::string path =
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat";

  // Check if file exists https://stackoverflow.com/a/6296808
  if (stat(path.c_str(), &buf) != 0)
    return true;

  QMessageBox confirm(p);

  QPushButton yes("YES", &confirm);
  QFont f = yes.font();
  f.setPointSize(11);
  yes.setFont(f);
  yes.setStyleSheet(*danger_button_style);
  QPushButton no("NO", &confirm);
  no.setFlat(true);
  no.setFont(f);

  confirm.setFont(f);
  confirm.setText("Existing diary found.");
  confirm.setInformativeText(
      "Are you sure you want to overwrite the existing diary?");
  confirm.addButton(&yes, QMessageBox::AcceptRole);
  confirm.addButton(&no, QMessageBox::RejectRole);
  confirm.setDefaultButton(&no);
  confirm.setTextInteractionFlags(Qt::NoTextInteraction);

  return QMessageBox::AcceptRole == confirm.exec();
}
