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
#include "encryptor.h"
#include "flushwindow.h"
#include "promptauth.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
  ui->setupUi(this);
  ui->options->hide();

  QString version;
  QString version_path(":/VERSION.txt");
  QFile version_file(version_path);

  version = version_file.open(QIODevice::ReadOnly)
                ? QString("Version ") + version_file.readAll()
                : QString("Unknown version");
  version_file.close();
  ui->version->setText(version);

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

void MainWindow::open_diary() {}

void MainWindow::new_diary() {}

void MainWindow::dl_diary() {
  connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_auth_err,
          this, &MainWindow::show_auth_err);
  connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_auth_ok,
          this, &MainWindow::_auth_ok);

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

void MainWindow::dump_drive() {}

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

void MainWindow::show_auth_err() {
  disconnect(TheoreticalDiary::instance()->gwrapper,
             &GoogleWrapper::sig_auth_err, this, &MainWindow::show_auth_err);

  AuthErrorWindow *w = new AuthErrorWindow(this);
  w->setModal(true);
  w->setAttribute(Qt::WA_DeleteOnClose, true);
  w->show();
}

void MainWindow::_auth_ok() {
  qDebug() << "Auth successful";
  disconnect(TheoreticalDiary::instance()->gwrapper,
             &GoogleWrapper::sig_auth_ok, this, &MainWindow::_auth_ok);
}

void MainWindow::quit_app() { close(); }

void MainWindow::closeEvent(QCloseEvent *event) {
  if (TheoreticalDiary::instance()->unsaved_changes) {
    QJsonDocument doc;
    QJsonObject tokens;

    tokens.insert("access_token",
                  TheoreticalDiary::instance()->gwrapper->google->token());
    tokens.insert(
        "refresh_token",
        TheoreticalDiary::instance()->gwrapper->google->refreshToken());
    doc.setObject(tokens);

    QFile file(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        "/credentials.json");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      file.write(doc.toJson());
      file.close();

      // If this fails, that's why the flush button is a thing :^)
    }
  }

  event->accept();
}
