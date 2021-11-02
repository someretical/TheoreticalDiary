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
#include "aboutwindow.h"
#include "autherrorwindow.h"
#include "confirmoverwrite.h"
#include "diarywindow.h"
#include "flushwindow.h"
#include "googlewrapper.h"
#include "missingpermissions.h"
#include "nodiaryfound.h"
#include "placeholder.h"
#include "promptauth.h"
#include "promptpassword.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"
#include "unknowndiaryformat.h"
#include "zipper.h"

#include <QDate>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <fstream>
#include <json.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
  ui->setupUi(this);

  // Set styles
  QFile file1(":/material_cyan_dark.qss");
  file1.open(QIODevice::ReadOnly);
  QString str1 = file1.readAll();
  file1.close();

  QFile file2(":/mainwindow.qss");
  file2.open(QIODevice::ReadOnly);
  QString str2 = file2.readAll();
  file2.close();
  setStyleSheet(str1 + str2);

  // Set version number
  file1.setFileName(":/VERSION.txt");
  file1.open(QIODevice::ReadOnly);
  str1 = file1.readAll();
  file1.close();
  ui->version->setText("Version " + str1);

  auto action = findChild<QAction *>("action_open");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::open_diary);

  action = findChild<QAction *>("action_new");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::new_diary);

  action = findChild<QAction *>("action_dl");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::dl_diary);

  action = findChild<QAction *>("action_import");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::import_diary);

  action = findChild<QAction *>("action_flush");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::flush_credentials);

  action = findChild<QAction *>("action_dump");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::dump_drive);

  action = findChild<QAction *>("action_about");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::about_app);

  action = findChild<QAction *>("action_quit");
  addAction(action);
  connect(action, &QAction::triggered, this, &MainWindow::quit_app,
          Qt::QueuedConnection);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::open_diary() {
  std::string contents;

  // Attempt to load file contents
  std::ifstream first(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat");

  if (!first.fail()) {
    // Taken from
    // https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    // Resizing the string upfront increases performance
    first.seekg(0, std::ios::end);
    contents.resize(first.tellg());
    first.seekg(0, std::ios::beg);
    first.read(contents.data(), contents.size());
    first.close();
  } else {
    std::ifstream second(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
            .toStdString() +
        "/TheoreticalDiary/diary.dat.bak");

    if (!second.fail()) {
      second.seekg(0, std::ios::end);
      contents.resize(second.tellg());
      second.seekg(0, std::ios::beg);
      second.read(contents.data(), contents.size());
      second.close();
    } else {
      // No valid file found
      NoDiaryFound w(this);
      w.exec();
      return;
    }
  }

  // If the user did not set a password, the string will only be Gzipped
  std::string decompressed;
  if (Zipper::unzip(contents, decompressed) &&
      TheoreticalDiary::instance()->diary_holder->load(decompressed)) {
    DiaryWindow w(this);
    w.exec();
    return;
  }

  // Prompt the user for a password
  std::string decrypted;
  PromptPassword w(contents, decrypted, this);
  if (w.exec() != QDialog::Accepted)
    return;

  // Attempt the Gunzip the decrypted contents
  if (!Zipper::unzip(decrypted, decompressed) ||
      !TheoreticalDiary::instance()->diary_holder->load(decompressed)) {
    UnknownDiaryFormat w(this);
    w.exec();
    return;
  }

  DiaryWindow w2(this);
  w2.exec();
}

void MainWindow::new_diary() {
  struct stat buf;
  std::string path =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat";

  // Check if file exists https://stackoverflow.com/a/6296808
  if (stat(path.c_str(), &buf) == 0) {
    ConfirmOverwrite w(this);

    if (w.exec() != QDialog::Accepted)
      return;
  }

  TheoreticalDiary::instance()->diary_holder->init();
  TheoreticalDiary::instance()->changes_made();

  DiaryWindow w2(this);
  w2.exec();
}

void MainWindow::dl_diary() {
  struct stat buf;
  std::string path =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat";

  if (stat(path.c_str(), &buf) == 0) {
    ConfirmOverwrite w(this);

    if (w.exec() != QDialog::Accepted)
      return;
  }

  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, this,
          &MainWindow::download_callback);

  /**
   * window.exec blocks any code AFTER it from executing.
   * window.show allows code after to run, but the new operator must be used.
   *
   * setAttribute(Qt::WA_DeleteOnClose, true);
   * makes Qt handle the deletion (not us).
   *
   * setModal(true);
   * makes the new window a child of the main window (not an entirely separate
   * window).
   */

  auto *w = new PromptAuth(this);
  w->setModal(true);
  w->setAttribute(Qt::WA_DeleteOnClose, true);
  w->show();

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void MainWindow::import_diary() {
  struct stat buf;
  std::string path =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          .toStdString() +
      "/TheoreticalDiary/diary.dat";

  if (stat(path.c_str(), &buf) == 0) {
    ConfirmOverwrite w(this);

    if (w.exec() != QDialog::Accepted)
      return;
  }

  auto filename =
      QFileDialog::getOpenFileName(this, "Import diary", QDir::homePath());
  if (filename.isEmpty())
    return;

  std::ifstream ifs(filename.toStdString());
  if (ifs.fail()) {
    MissingPermissions w(this);
    w.exec();
    return;
  }

  std::string content;
  ifs.seekg(0, std::ios::end);
  content.resize(ifs.tellg());
  ifs.seekg(0, std::ios::beg);
  ifs.read(content.data(), content.size());
  ifs.close();

  if (TheoreticalDiary::instance()->diary_holder->load(content)) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    TheoreticalDiary::instance()->encryptor->regenerate_salt();
    TheoreticalDiary::instance()->encryptor->set_key("a");
    TheoreticalDiary::instance()->changes_made();
    QApplication::restoreOverrideCursor();

    DiaryWindow w(this);
    w.exec();
  } else {
    UnknownDiaryFormat w(this);
    w.exec();
  }
}

void MainWindow::flush_credentials() {
  QFile file(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      "/TheoreticalDiary/credentials.json");
  file.remove();

  TheoreticalDiary::instance()->gwrapper->google->unlink();

  FlushWindow w(this);
  w.exec();
}

void MainWindow::dump_drive() {
  Placeholder w(this);
  w.exec();
}

void MainWindow::about_app() {
  AboutWindow w(this);
  w.exec();
}

void MainWindow::download_callback(const td::Res code) {
  disconnect(TheoreticalDiary::instance()->gwrapper,
             &GoogleWrapper::sig_oauth2_callback, this,
             &MainWindow::download_callback);

  if (td::Res::No == code) {
    auto *w = new AuthErrorWindow(this);
    w->setModal(true);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->show();
    return;
  }

  //  // Google drive testing
  //  auto file = new QFile();
  //  auto params = new QVariantMap();

  //  // diary.dat 17f3nXYByqW7Xf0WD5ujSe8uwiAGRiLbONSWd9V8LKynnmIS2
  //  // diary.dat.bak 1X0BVcUn3SeMgkilRAn0qx2EalW3HvvSWhbINhlxy4MbM4Tvm
  //  // List files

  //  params->insert("spaces", "appDataFolder");

  //  auto reply1 = TheoreticalDiary::instance()->gwrapper->google->get(
  //      QUrl("https://www.googleapis.com/drive/v3/files"), *params);

  //  connect(reply1, &QNetworkReply::finished, [=]() {
  //    params->clear();
  //    if (QNetworkReply::NoError != reply1->error()) {
  //      qDebug() << "network err1" << reply1->error();
  //      return;
  //    }

  //    auto json =
  //        nlohmann::json::parse(reply1->readAll().toStdString(), nullptr,
  //        false);
  //    if (json.is_discarded()) {
  //      qDebug() << "failed to parse json";
  //      return;
  //    }

  //    auto vec = json.at("files");
  //    auto res = std::find_if(
  //        vec.begin(), vec.end(), [](const nlohmann::json &file_obj) {
  //          auto it = file_obj.find("name");
  //          return it != file_obj.end() && it.value() == "diary.dat";
  //        });

  //    if (res == vec.end()) {
  //      qDebug() << "failed to find diary.dat";
  //      return;
  //    }

  //    qDebug() << "successfully found fileID";
  //    reply1->deleteLater();
  //  });

  //
  //
  // Download files
  //  TheoreticalDiary::instance()->gwrapper->google->refreshAccessToken();
  //  new DriveDownloader(
  //      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
  //      +
  //          "/TheoreticalDiary/test_download.dat",
  //      "17f3nXYByqW7Xf0WD5ujSe8uwiAGRiLbONSWd9V8LKynnmIS2", this);
}

void MainWindow::quit_app() { close(); }

void MainWindow::closeEvent(QCloseEvent *event) {
  if (TheoreticalDiary::instance()->unsaved_changes) {
    //    if (*TheoreticalDiary::instance()->gwrapper->contains_valid_info)
    //      TheoreticalDiary::instance()->gwrapper->save_credentials();

    TheoreticalDiary::instance()->save_settings();
  }

  event->accept();
}
