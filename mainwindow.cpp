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
#include "aboutwindow.h"
#include "autherrorwindow.h"
#include "diarywindow.h"
#include "encryptor.h"
#include "flushwindow.h"
#include "nodiaryfound.h"
#include "promptauth.h"
#include "promptpassword.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"
#include "unknowndiaryformat.h"
#include "zipper.h"

#include <QCloseEvent>
#include <QStandardPaths>
#include <fstream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
  ui->setupUi(this);
  ui->options->hide();

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();

  QFile ss_file_2(":/styles/mainwindow.qss");
  ss_file_2.open(QIODevice::ReadOnly);
  QString stylesheet2 = ss_file_2.readAll();

  setStyleSheet(stylesheet + stylesheet2);

  QFile version_file(":/text/VERSION.txt");
  version_file.open(QIODevice::ReadOnly);
  QString version = version_file.readAll();
  version_file.close();
  ui->version->setText("Version " + version);

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

  action = findChild<QAction *>("action_options");
  addAction(action);
  connect(action, &QAction::triggered, this,
          &MainWindow::toggle_advanced_options);

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
  std::string uncompressed;

  auto success = Zipper::unzip(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
              .toStdString() +
          "/diary.dat",
      uncompressed);

  if (!success) {
    auto fallback = Zipper::unzip(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                .toStdString() +
            "/diary.dat.bak",
        uncompressed);

    if (!fallback) {
      NoDiaryFound *w = new NoDiaryFound(this);
      w->setModal(true);
      w->setAttribute(Qt::WA_DeleteOnClose, true);
      w->show();
      return;
    }
  }

  success = TheoreticalDiary::instance()->diary_holder->load(uncompressed);

  if (success) {
    DiaryWindow w(this);
    w.show();
  } else {
    PromptPassword w(uncompressed, this);
    w.exec();
  }
}

void MainWindow::prompt_pwd_callback(const int code) {
  if (code) {
    UnknownDiaryFormat *w = new UnknownDiaryFormat(this);
    w->setModal(true);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->show();
  } else {
    DiaryWindow w(this);
    w.show();
  }
}

void MainWindow::new_diary() {}

void MainWindow::dl_diary() {
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, this,
          &MainWindow::oauth2_callback);

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

  PromptAuth *w = new PromptAuth(this);
  w->setModal(true);
  w->setAttribute(Qt::WA_DeleteOnClose, true);
  w->show();

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void MainWindow::import_diary() {}

void MainWindow::flush_credentials() {
  QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
             "/credentials.json");
  file.remove();

  FlushWindow w(this);
  w.exec();
}

void MainWindow::dump_drive() {
  std::string path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
          .toStdString() +
      "/test.dat";
  std::string pwd = "abc12345";
  std::string text = "a very secret message";

  std::vector<CryptoPP::byte> pwd_hash;
  std::string encrypted;
  Encryptor::get_hash(pwd, pwd_hash);
  Encryptor::encrypt(pwd_hash, text, encrypted);

  if (Zipper::zip(path, encrypted)) {
    qDebug() << "Written to file";
    qDebug() << "Encrypted:" << QString::fromStdString(encrypted);

    std::string uncompressed;
    std::string decrypted;

    if (Zipper::unzip(path, uncompressed)) {
      qDebug() << "Encrypted:" << QString::fromStdString(uncompressed);

      Encryptor::decrypt(pwd_hash, uncompressed, decrypted);

      qDebug() << "Uncompressed and decrypted:"
               << QString::fromStdString(decrypted);
    } else {
      qDebug() << "Failed to unzip";
    }
  } else {
    qDebug() << "Failed to write to file";
  }
}

void MainWindow::about_app() {
  AboutWindow w(this);
  w.exec();
}

void MainWindow::toggle_advanced_options() {
  if (ui->options->isVisible())
    ui->options->hide();
  else
    ui->options->show();
}

void MainWindow::oauth2_callback(const int code) {
  disconnect(TheoreticalDiary::instance()->gwrapper,
             &GoogleWrapper::sig_oauth2_callback, this,
             &MainWindow::oauth2_callback);

  if (code) {
    AuthErrorWindow *w = new AuthErrorWindow(this);
    w->setModal(true);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->show();
  } else {
    qDebug() << "Auth successful";
  }
}

void MainWindow::quit_app() { close(); }

void MainWindow::closeEvent(QCloseEvent *event) {
  if (TheoreticalDiary::instance()->unsaved_changes) {
    TheoreticalDiary::instance()->gwrapper->save_credentials();
    TheoreticalDiary::instance()->settings_provider->save();
  }

  event->accept();
}
