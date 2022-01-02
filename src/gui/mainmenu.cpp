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

#include "mainmenu.h"
#include "../core/theoreticaldiary.h"
#include "../util/zipper.h"
#include "aboutdialog.h"
#include "mainwindow.h"
#include "ui_mainmenu.h"

#include <fstream>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu) {
  ui->setupUi(this);
  ui->version->setText("Version " + QApplication::applicationVersion());
  ui->alert_text->setText("");

  QTimer::singleShot(0, [&]() { ui->password_box->setFocus(); });

  // Pressing enter will try to decrypt
  enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
  connect(
      enter, &QShortcut::activated, this,
      [&]() { QMetaObject::invokeMethod(ui->decrypt_button, "clicked"); },
      Qt::QueuedConnection);

  connect(ui->decrypt_button, &QPushButton::clicked, this,
          &MainMenu::decrypt_diary, Qt::QueuedConnection);
  connect(ui->new_button, &QPushButton::clicked, this, &MainMenu::new_diary,
          Qt::QueuedConnection);
  connect(ui->import_button, &QPushButton::clicked, this,
          &MainMenu::import_diary, Qt::QueuedConnection);
  connect(ui->options_button, &QPushButton::clicked, this,
          &MainMenu::open_options, Qt::QueuedConnection);
  connect(ui->quit_button, &QPushButton::clicked,
          qobject_cast<MainWindow *>(parent), &MainWindow::close,
          Qt::QueuedConnection);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &MainMenu::apply_theme, Qt::QueuedConnection);
  apply_theme();
}

MainMenu::~MainMenu() {
  delete ui;
  delete enter;
}

void MainMenu::apply_theme() {
  QFile file(":/global/mainmenu.qss");
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void MainMenu::open_options() {
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_options_menu();
}

bool MainMenu::get_diary_contents() {
  // Attempt to load file contents
  std::ifstream first(
      TheoreticalDiary::instance()->data_location().toStdString() +
      "/diary.dat");
  auto &str = TheoreticalDiary::instance()->encryptor->encrypted_str;

  if (!first.fail()) {
    // Taken from
    // https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    // Resizing the string upfront increases performance
    first.seekg(0, std::ios::end);
    str->resize(first.tellg());
    first.seekg(0, std::ios::beg);
    first.read(str->data(), str->size());
    first.close();
  } else {
    std::ifstream second(
        TheoreticalDiary::instance()->data_location().toStdString() +
        "/diary.dat.bak");

    if (!second.fail()) {
      second.seekg(0, std::ios::end);
      str->resize(second.tellg());
      second.seekg(0, std::ios::beg);
      second.read(str->data(), str->size());
      second.close();
    } else {
      // No valid file found
      QMessageBox rip(this);
      QPushButton ok_button("OK", &rip);
      ok_button.setFlat(true);
      QFont f = ok_button.font();
      f.setPointSize(11);
      ok_button.setFont(f);

      rip.setFont(f);
      rip.setText("No diary was found.");
      rip.setInformativeText("You can create a new diary by clicking the "
                             "\"New\" button in the main menu.");
      rip.addButton(&ok_button, QMessageBox::AcceptRole);
      rip.setDefaultButton(&ok_button);
      rip.setTextInteractionFlags(Qt::NoTextInteraction);
      rip.exec();

      ui->decrypt_button->setEnabled(true);
      ui->import_button->setEnabled(true);
      ui->new_button->setEnabled(true);
      ui->options_button->setEnabled(true);

      return false;
    }
  }

  return true;
}

void MainMenu::decrypt_diary() {
  if (!ui->decrypt_button->isEnabled())
    return;

  ui->decrypt_button->setEnabled(false);
  ui->import_button->setEnabled(false);
  ui->new_button->setEnabled(false);
  ui->options_button->setEnabled(false);

  const auto &password = ui->password_box->text().toStdString();
  ui->password_box->setText("");

  const auto &opt = get_diary_contents();
  if (!opt)
    return;

  // If the password box is empty, try to decompress immediately.
  if (password.empty()) {
    return decrypt_diary_cb(false);
  }

  auto &str = TheoreticalDiary::instance()->encryptor->encrypted_str;
  TheoreticalDiary::instance()->encryptor->set_salt(str->substr(0, SALT_SIZE));
  str->erase(0, SALT_SIZE);
  TheoreticalDiary::instance()->encryptor->set_decrypt_iv(
      str->substr(0, IV_SIZE));
  str->erase(0, IV_SIZE);

  ui->alert_text->setText("Decrypting...");
  ui->alert_text->update();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  auto worker = new HashWorker();
  worker->moveToThread(&TheoreticalDiary::instance()->worker_thread);
  connect(&TheoreticalDiary::instance()->worker_thread, &QThread::finished,
          worker, &QObject::deleteLater, Qt::QueuedConnection);
  connect(worker, &HashWorker::done, this, &MainMenu::decrypt_diary_cb,
          Qt::QueuedConnection);
  connect(TheoreticalDiary::instance(), &TheoreticalDiary::sig_begin_hash,
          worker, &HashWorker::hash, Qt::QueuedConnection);

  // The signal must be emitted to ensure the thread is run properly
  emit TheoreticalDiary::instance()->sig_begin_hash(password);
}

void MainMenu::decrypt_diary_cb(const bool do_decrypt) {
  std::string decrypted;
  if (do_decrypt) {
    const auto &res = TheoreticalDiary::instance()->encryptor->decrypt(
        *TheoreticalDiary::instance()->encryptor->encrypted_str);
    if (!res) {
      ui->alert_text->setText("Wrong password.");
      ui->decrypt_button->setEnabled(true);
      ui->import_button->setEnabled(true);
      ui->new_button->setEnabled(true);
      ui->options_button->setEnabled(true);
      return QApplication::restoreOverrideCursor();
    }

    decrypted.assign(*res);
  }

  std::string decompressed;
  if (Zipper::unzip(
          do_decrypt ? decrypted
                     : *TheoreticalDiary::instance()->encryptor->encrypted_str,
          decompressed) &&
      TheoreticalDiary::instance()->diary_holder->load(decompressed)) {
    qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->show_diary_menu(QDate::currentDate());
    return QApplication::restoreOverrideCursor();
  }

  ui->alert_text->setText("Unable to parse diary.");
  ui->alert_text->update();
  ui->decrypt_button->setEnabled(true);
  ui->import_button->setEnabled(true);
  ui->new_button->setEnabled(true);
  ui->options_button->setEnabled(true);
  QApplication::restoreOverrideCursor();
}

void MainMenu::new_diary() {
  if (!TheoreticalDiary::instance()->confirm_overwrite(this))
    return;

  TheoreticalDiary::instance()->encryptor->reset();
  TheoreticalDiary::instance()->diary_holder->init();
  TheoreticalDiary::instance()->diary_changed();

  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_diary_menu(QDate::currentDate());
}

void MainMenu::import_diary() {
  if (!TheoreticalDiary::instance()->confirm_overwrite(this))
    return;

  const auto &filename = QFileDialog::getOpenFileName(
      this, "Import diary", QDir::homePath(), "JSON (*.json);;All files");
  if (filename.isEmpty())
    return;

  std::ifstream ifs(filename.toStdString());
  if (ifs.fail()) {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
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
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
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
