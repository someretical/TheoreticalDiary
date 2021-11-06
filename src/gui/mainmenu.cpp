/*
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

#include "mainmenu.h"
#include "../core/theoreticaldiary.h"
#include "../util/zipper.h"
#include "aboutdialog.h"
#include "mainwindow.h"
#include "promptpassword.h"
#include "ui_mainmenu.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <fstream>
#include <string>
#include <sys/stat.h>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu) {
  ui->setupUi(this);
  ui->version->setText("Version " + QApplication::applicationVersion());

  QTimer::singleShot(0, [&]() {
    QApplication::restoreOverrideCursor();
    qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->resize(800, 600);
  });

  connect(ui->open_button, &QPushButton::clicked, this, &MainMenu::open_diary);
  connect(ui->new_button, &QPushButton::clicked, this, &MainMenu::new_diary);
  connect(ui->import_button, &QPushButton::clicked, this,
          &MainMenu::import_diary);
  connect(ui->download_button, &QPushButton::clicked, this,
          &MainMenu::download_diary);
  connect(ui->flush_button, &QPushButton::clicked, this,
          &MainMenu::flush_credentials);
  connect(ui->dump_button, &QPushButton::clicked, this, &MainMenu::dump_drive);
  connect(ui->about_button, &QPushButton::clicked, this, &MainMenu::view_info);
  connect(ui->quit_button, &QPushButton::clicked,
          qobject_cast<MainWindow *>(parent), &MainWindow::close);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &MainMenu::apply_theme);
  apply_theme();
}

MainMenu::~MainMenu() { delete ui; }

void MainMenu::apply_theme() {
  QFile file(":/" + TheoreticalDiary::instance()->theme() + "/mainmenu.qss");
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void MainMenu::open_diary() {
  std::string contents;

  // Attempt to load file contents
  std::ifstream first(
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat");

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
        TheoreticalDiary::instance()->data_location().toStdString() +
        "/diary.dat.bak");

    if (!second.fail()) {
      second.seekg(0, std::ios::end);
      contents.resize(second.tellg());
      second.seekg(0, std::ios::beg);
      second.read(contents.data(), contents.size());
      second.close();
    } else {
      // No valid file found
      QMessageBox rip(this);
      QPushButton ok_button("OK", &rip);
      ok_button.setFlat(true);

      rip.setText("No diary was found.");
      rip.setInformativeText("You can create a new diary by clicking the "
                             "\"New\" button in the main menu.");
      rip.addButton(&ok_button, QMessageBox::AcceptRole);
      rip.setDefaultButton(&ok_button);
      rip.setTextInteractionFlags(Qt::NoTextInteraction);

      rip.exec();
      return;
    }
  }

  // If the user did not set a password, the string will only be Gzipped
  std::string decompressed;
  if (Zipper::unzip(contents, decompressed) &&
      TheoreticalDiary::instance()->diary_holder->load(decompressed)) {
    qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->show_diary_menu(QDate::currentDate());
    return;
  }

  // Prompt the user for a password
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_password_prompt(contents);
}

bool MainMenu::confirm_overwrite() {
  struct stat buf;
  std::string path =
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat";

  // Check if file exists https://stackoverflow.com/a/6296808
  if (stat(path.c_str(), &buf) != 0)
    return true;

  QMessageBox confirm(this);

  QPushButton yes("YES", &confirm);
  yes.setStyleSheet(*qobject_cast<MainWindow *>(parentWidget()->parentWidget())
                         ->danger_button_style);
  QPushButton no("NO", &confirm);
  no.setFlat(true);

  confirm.setText("Existing diary found.");
  confirm.setInformativeText(
      "Are you sure you want to overwrite the existing diary?");
  confirm.addButton(&yes, QMessageBox::AcceptRole);
  confirm.addButton(&no, QMessageBox::RejectRole);
  confirm.setDefaultButton(&no);
  confirm.setTextInteractionFlags(Qt::NoTextInteraction);

  return confirm.exec() == QMessageBox::AcceptRole;
}

void MainMenu::new_diary() {
  if (!confirm_overwrite())
    return;

  TheoreticalDiary::instance()->encryptor->reset();
  TheoreticalDiary::instance()->diary_holder->init();
  TheoreticalDiary::instance()->diary_changed();

  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_diary_menu(QDate::currentDate());
}

void MainMenu::import_diary() {
  if (!confirm_overwrite())
    return;

  auto filename =
      QFileDialog::getOpenFileName(this, "Import diary", QDir::homePath());
  if (filename.isEmpty())
    return;

  std::ifstream ifs(filename.toStdString());
  if (ifs.fail()) {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);

    rip.setText("Read error.");
    rip.setInformativeText(
        "The app was unable to read the contents of the file.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
    return;
  }

  std::string content;
  ifs.seekg(0, std::ios::end);
  content.resize(ifs.tellg());
  ifs.seekg(0, std::ios::beg);
  ifs.read(content.data(), content.size());
  ifs.close();

  if (!TheoreticalDiary::instance()->diary_holder->load(content)) {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);

    rip.setText("Parsing error.");
    rip.setInformativeText(
        "The app was unable to read the contents of the diary.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
    return;
  }

  TheoreticalDiary::instance()->diary_changed();
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_diary_menu(QDate::currentDate());
}

void MainMenu::download_diary() {
  if (!confirm_overwrite())
    return;

  QMessageBox placeholder(this);
  QPushButton ok_button("OK", &placeholder);
  ok_button.setFlat(true);
  placeholder.setText("Placeholder dialog");
  placeholder.addButton(&ok_button, QMessageBox::AcceptRole);
  placeholder.setTextInteractionFlags(Qt::NoTextInteraction);
  placeholder.exec();
}

void MainMenu::flush_credentials() {
  QMessageBox placeholder(this);
  QPushButton ok_button("OK", &placeholder);
  ok_button.setFlat(true);
  placeholder.setText("Placeholder dialog");
  placeholder.addButton(&ok_button, QMessageBox::AcceptRole);
  placeholder.setTextInteractionFlags(Qt::NoTextInteraction);
  placeholder.exec();
}

void MainMenu::dump_drive() {
  QMessageBox placeholder(this);
  QPushButton ok_button("OK", &placeholder);
  ok_button.setFlat(true);
  placeholder.setText("Placeholder dialog");
  placeholder.addButton(&ok_button, QMessageBox::AcceptRole);
  placeholder.setTextInteractionFlags(Qt::NoTextInteraction);
  placeholder.exec();
}

void MainMenu::view_info() {
  AboutDialog w(this);
  w.exec();
}
