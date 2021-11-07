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

#include "diarymenu.h"
#include "../core/theoreticaldiary.h"
#include "diaryeditor.h"
#include "diaryentryviewer.h"
#include "diarypixels.h"
#include "diarystats.h"
#include "mainwindow.h"
#include "ui_diarymenu.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <fstream>
#include <json.hpp>

DiaryMenu::DiaryMenu(const QDate &date, QWidget *parent)
    : QWidget(parent), ui(new Ui::DiaryMenu) {
  ui->setupUi(this);

  ui->sync_button->setChecked(
      TheoreticalDiary::instance()->local_settings->sync_enabled);
  ui->sync_button->update();

  // Ctrl S to save the diary
  save_shortcut = new QShortcut(QKeySequence::Save, this);
  save_shortcut->setAutoRepeat(false);
  connect(save_shortcut, &QShortcut::activated, this, &DiaryMenu::save_diary);

  first_created = new QDate(date);

  // When changes are made in the editor, the other tabs need to know about it
  // so they can update accordingly.
  auto diary_editor = new DiaryEditor(this);
  ui->editor->layout()->addWidget(diary_editor);
  ui->entries->layout()->addWidget(new DiaryEntryViewer(diary_editor, this));
  ui->statistics->layout()->addWidget(new DiaryStats(diary_editor, this));
  ui->pixels->layout()->addWidget(new DiaryPixels(diary_editor, this));

  connect(ui->close_button, &QPushButton::clicked, this,
          &DiaryMenu::close_window);
  connect(ui->pwd_button, &QPushButton::clicked, this,
          &DiaryMenu::change_password);
  connect(ui->export_button, &QPushButton::clicked, this,
          &DiaryMenu::export_diary);
  connect(ui->sync_button, &QPushButton::clicked, this,
          &DiaryMenu::toggle_sync);
  connect(ui->tabWidget, &QTabWidget::currentChanged, this,
          &DiaryMenu::tab_changed);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &DiaryMenu::apply_theme);
  apply_theme();
}

DiaryMenu::~DiaryMenu() {
  delete ui;
  delete save_shortcut;
  delete first_created;
}

void DiaryMenu::apply_theme() {
  QFile file(
      QString(":/%1/diarymenu.qss").arg(TheoreticalDiary::instance()->theme()));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void DiaryMenu::tab_changed(const int &tab) {
  switch (ui->tabWidget->currentIndex()) {
  // Editor tab
  case 0:
    break;
  // List tab
  case 1:
    break;
  // Stats tab
  case 2:
    break;
  // Pixels tab
  case 3:
    break;
  }
}

QString DiaryMenu::get_day_suffix(const int &day) {
  switch (day) {
  case 1:
  case 21:
  case 31:
    return "st";
  case 2:
  case 22:
    return "nd";
  case 3:
  case 23:
    return "rd";
  default:
    return "th";
  }
}

void DiaryMenu::close_window() {
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())->close();
}

void DiaryMenu::change_password() {
  qobject_cast<MainWindow *>(parentWidget()->parentWidget())
      ->show_update_password_prompt(*first_created);
}

void DiaryMenu::export_diary() {
  auto filename =
      QFileDialog::getSaveFileName(this, "Export diary", QDir::homePath());

  if (filename.isEmpty())
    return;

  std::ofstream dst(filename.toStdString());

  if (!dst.fail()) {
    nlohmann::json j = *(TheoreticalDiary::instance()->diary_holder->diary);
    dst << j.dump(4);
    dst.close();

    QMessageBox ok(this);
    QPushButton ok_button("OK", &ok);
    ok_button.setFlat(true);

    ok.setText("Diary exported.");
    ok.addButton(&ok_button, QMessageBox::AcceptRole);
    ok.setDefaultButton(&ok_button);
    ok.setTextInteractionFlags(Qt::NoTextInteraction);

    ok.exec();
  } else {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);

    rip.setText("Export failed.");
    rip.setInformativeText(
        "The app could not write to the specified location.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
  }
}

void DiaryMenu::toggle_sync() {
  TheoreticalDiary::instance()->local_settings->sync_enabled =
      ui->sync_button->isChecked();
  TheoreticalDiary::instance()->local_settings_changed();
}

void DiaryMenu::save_diary() {
  if (!qobject_cast<MainWindow *>(parentWidget()->parentWidget())->save_diary())
    return;

  QMessageBox ok(this);
  QPushButton ok_button("OK", &ok);
  ok_button.setFlat(true);

  ok.setText("Diary saved.");
  ok.addButton(&ok_button, QMessageBox::AcceptRole);
  ok.setDefaultButton(&ok_button);
  ok.setTextInteractionFlags(Qt::NoTextInteraction);

  ok.exec();
}
