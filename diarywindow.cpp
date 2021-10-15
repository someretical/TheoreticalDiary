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

#include "diarywindow.h"
#include "encryptor.h"
#include "saveerror.h"
#include "theoreticaldiary.h"
#include "ui_diarywindow.h"
#include "unsavedchanges.h"
#include "updatepassword.h"
#include "zipper.h"

#include <ctime>
#include <fstream>
#include <string>

DiaryWindow::DiaryWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::DiaryWindow) {
  ui->setupUi(this);

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  auto action = findChild<QAction *>("action_save");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::action_save);

  action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::reject);

  action = findChild<QAction *>("action_update_password");
  addAction(action);
  connect(action, &QAction::triggered, this, &DiaryWindow::update_password);
}

DiaryWindow::~DiaryWindow() { delete ui; }

void DiaryWindow::reject() {
  if (*(TheoreticalDiary::instance()->unsaved_changes)) {
    UnsavedChanges<DiaryWindow> w(&DiaryWindow::confirm_close_callback, this);
    w.exec();
  } else {
    confirm_close_callback(0);
  }
}

void DiaryWindow::confirm_close_callback(const int code) {
  if (code == 1)
    return;

  *(TheoreticalDiary::instance()->unsaved_changes) = false;
  TheoreticalDiary::instance()->diary_holder->key->clear();
  TheoreticalDiary::instance()->diary_holder->diary->clear();

  accept();
}

void DiaryWindow::action_save() {
  auto stringified = TheoreticalDiary::instance()->diary_holder->diary->dump();
  auto key = TheoreticalDiary::instance()->diary_holder->key;

  std::string primary_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/diary.dat";
  std::string backup_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/diary.dat.bak";
  std::string final;

  // Backup existing diary first
  std::ifstream src(primary_path, std::ios::binary);
  if (!src.fail()) {
    std::ofstream dst(backup_path, std::ios::binary);
    dst << src.rdbuf();
    dst.close();
  }
  src.close();

  // Update last_updated
  (*TheoreticalDiary::instance()
        ->diary_holder->diary)["metadata"]["last_updated"] = std::time(nullptr);

  // If there is a password set, encrypt the diary
  if (key->size() == 32) {
    Encryptor::encrypt(*key, stringified, final);
  } else {
    final = stringified;
  }

  auto success = Zipper::zip(primary_path, final);
  if (!success) {
    SaveError w(this);
    w.exec();
  } else {
    *(TheoreticalDiary::instance()->unsaved_changes) = false;
  }
}

void DiaryWindow::update_password() {
  UpdatePassword w(this);
  w.exec();
}
