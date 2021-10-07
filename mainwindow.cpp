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
#include "flushwindow.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
  responsive = true;
  ui->setupUi(this);

  // Setup normal buttons
  connect(ui->open_button, SIGNAL(clicked()), this,
          SLOT(open_button_pressed()));
  connect(ui->new_button, SIGNAL(clicked()), this, SLOT(new_button_pressed()));
  connect(ui->dl_button, SIGNAL(clicked()), this, SLOT(dl_button_pressed()));
  connect(ui->import_button, SIGNAL(clicked()), this,
          SLOT(import_button_pressed()));
  connect(ui->flush_button, SIGNAL(clicked()), this,
          SLOT(flush_button_pressed()));
  connect(ui->dump_button, SIGNAL(clicked()), this,
          SLOT(dump_button_pressed()));
  connect(ui->about_button, SIGNAL(clicked()), this,
          SLOT(about_button_pressed()));

  // Setup expandable options
  ui->options->hide();
  connect(ui->options_button, SIGNAL(clicked()), this,
          SLOT(toggle_advanced_options()));

  // Setup quit action
  auto action_quit = this->findChild<QAction *>("action_quit");
  addAction(action_quit);
  connect(action_quit, SIGNAL(triggered()), this, SLOT(quit_app()),
          Qt::QueuedConnection);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::open_button_pressed() {
  if (!responsive)
    return;
}

void MainWindow::new_button_pressed() {
  if (!responsive)
    return;
}

void MainWindow::dl_button_pressed() {
  if (!responsive)
    return;

  responsive = false;
  connect(TheoreticalDiary::instance()->gwrapper, SIGNAL(sig_auth_err()), this,
          SLOT(show_auth_err()));
  connect(TheoreticalDiary::instance()->gwrapper, SIGNAL(sig_auth_ok()), this,
          SLOT(_auth_ok()));
  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void MainWindow::import_button_pressed() {
  if (!responsive)
    return;
}

void MainWindow::flush_button_pressed() {
  if (!responsive)
    return;

  responsive = false;
  QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
             "/credentials.json");
  file.remove();

  FlushWindow flush_window(this);
  flush_window.exec();
  responsive = true;
}

void MainWindow::dump_button_pressed() {
  if (!responsive)
    return;
}

void MainWindow::about_button_pressed() {
  if (!responsive)
    return;

  AboutWindow about_window(this);
  about_window.exec();
}

void MainWindow::toggle_advanced_options() {
  if (!responsive)
    return;

  if (ui->options->isVisible())
    ui->options->hide();
  else
    ui->options->show();
}

void MainWindow::make_responsive() { responsive = true; }

void MainWindow::show_auth_err() {
  AuthErrorWindow auth_error_window(this);
  auth_error_window.exec();

  disconnect(TheoreticalDiary::instance()->gwrapper, SIGNAL(sig_auth_err()),
             this, SLOT(show_auth_err()));
  responsive = true;
}

void MainWindow::_auth_ok() {
  qDebug() << "Auth successful";
  responsive = true;
  disconnect(TheoreticalDiary::instance()->gwrapper, SIGNAL(sig_auth_ok()),
             this, SLOT(_auth_ok()));
}

void MainWindow::quit_app() { qApp->quit(); }
