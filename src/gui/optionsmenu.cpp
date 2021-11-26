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

#include "optionsmenu.h"
#include "../core/theoreticaldiary.h"
#include "aboutdialog.h"
#include "apiresponse.h"
#include "licensesdialog.h"
#include "mainwindow.h"
#include "ui_optionsmenu.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPushButton>
#include <fstream>

OptionsMenu::OptionsMenu(bool from_diary_editor, QWidget *parent)
    : QWidget(parent), ui(new Ui::OptionsMenu) {
  ui->setupUi(this);

  diary_editor_mode = from_diary_editor;

  connect(ui->ok_button, &QPushButton::clicked, this, &OptionsMenu::back,
          Qt::QueuedConnection);
  connect(ui->apply_button, &QPushButton::clicked, this,
          &OptionsMenu::save_settings, Qt::QueuedConnection);
  connect(ui->export_button, &QPushButton::clicked, this,
          &OptionsMenu::export_diary, Qt::QueuedConnection);
  connect(ui->change_password_button, &QPushButton::clicked, this,
          &OptionsMenu::change_password, Qt::QueuedConnection);
  connect(ui->download_backup_button, &QPushButton::clicked, this,
          &OptionsMenu::download_backup, Qt::QueuedConnection);
  connect(ui->upload_backup_button, &QPushButton::clicked, this,
          &OptionsMenu::upload_diary, Qt::QueuedConnection);
  connect(ui->flush_oauth_button, &QPushButton::clicked, this,
          &OptionsMenu::flush_oauth, Qt::QueuedConnection);
  connect(ui->dev_list_files_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_list, Qt::QueuedConnection);
  connect(ui->dev_upload_file_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_upload, Qt::QueuedConnection);
  connect(ui->dev_download_file_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_download, Qt::QueuedConnection);
  connect(ui->dev_update_file_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_update, Qt::QueuedConnection);
  connect(ui->dev_copy_file_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_copy, Qt::QueuedConnection);
  connect(ui->dev_delete_button, &QPushButton::clicked, this,
          &OptionsMenu::dev_delete, Qt::QueuedConnection);
  connect(ui->about_button, &QPushButton::clicked, this,
          &OptionsMenu::show_about, Qt::QueuedConnection);
  connect(ui->licenses_button, &QPushButton::clicked, this,
          &OptionsMenu::show_licenses, Qt::QueuedConnection);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &OptionsMenu::apply_theme, Qt::QueuedConnection);
  apply_theme();

  setup_layout();
}

OptionsMenu::~OptionsMenu() { delete ui; }

void OptionsMenu::apply_theme() {
  auto theme = TheoreticalDiary::instance()->theme();

  QFile file(QString(":/%1/optionsmenu.qss").arg(theme));
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}

void OptionsMenu::back() {
  if (diary_editor_mode)
    return qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->show_diary_menu(QDate::currentDate());

  qobject_cast<MainWindow *>(parentWidget()->parentWidget())->show_main_menu();
}

void OptionsMenu::save_settings() {
  TheoreticalDiary::instance()->settings->setValue(
      "sync_enabled", ui->sync_checkbox->isChecked());

  if (diary_editor_mode)
    qobject_cast<MainWindow *>(parentWidget()->parentWidget())
        ->save_diary(false);

  back();
}

void OptionsMenu::setup_layout() {
  ui->alert_text->setText("");

  if (!diary_editor_mode) {
    ui->export_button->setEnabled(false);
    ui->change_password_button->setEnabled(false);
    ui->new_password->setEnabled(false);
    ui->new_password_confirm->setEnabled(false);
  }

  ui->sync_checkbox->setChecked(TheoreticalDiary::instance()
                                    ->settings->value("sync_enabled", false)
                                    .toBool());
}

void OptionsMenu::export_diary() {
  auto filename = QFileDialog::getSaveFileName(
      this, "Export diary", QString("%1/export.json").arg(QDir::homePath()),
      "JSON (*.json);;All files");

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
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    ok.setFont(f);
    ok.setText("Diary exported.");
    ok.addButton(&ok_button, QMessageBox::AcceptRole);
    ok.setDefaultButton(&ok_button);
    ok.setTextInteractionFlags(Qt::NoTextInteraction);

    ok.exec();
  } else {
    QMessageBox rip(this);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
    rip.setText("Export failed.");
    rip.setInformativeText(
        "The app could not write to the specified location.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    rip.exec();
  }
}

void OptionsMenu::change_password() {
  ui->change_password_button->setEnabled(false);
  ui->apply_button->setEnabled(false);
  ui->ok_button->setEnabled(false);
  TheoreticalDiary::instance()->closeable = false;
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  ui->alert_text->setText("");
  ui->alert_text->update();

  auto password = ui->new_password->text();
  if (password != ui->new_password_confirm->text()) {
    ui->alert_text->setText("The passwords do not match.");
    ui->alert_text->update();

    QApplication::restoreOverrideCursor();
    return ui->change_password_button->setEnabled(true);
  }

  if (0 != password.length()) {
    ui->alert_text->setText("Changing password...");
    ui->alert_text->update();

    auto worker = new HashWorker();
    worker->moveToThread(&TheoreticalDiary::instance()->worker_thread);
    connect(&TheoreticalDiary::instance()->worker_thread, &QThread::finished,
            worker, &QObject::deleteLater, Qt::QueuedConnection);
    connect(worker, &HashWorker::done, this, &OptionsMenu::change_password_cb,
            Qt::QueuedConnection);
    connect(TheoreticalDiary::instance(), &TheoreticalDiary::sig_begin_hash,
            worker, &HashWorker::hash, Qt::QueuedConnection);

    TheoreticalDiary::instance()->encryptor->regenerate_salt();
    emit TheoreticalDiary::instance()->sig_begin_hash(password.toStdString());
  } else {
    TheoreticalDiary::instance()->encryptor->reset();
    change_password_cb();
  }
}

void OptionsMenu::change_password_cb() {
  TheoreticalDiary::instance()->diary_changed();
  QApplication::restoreOverrideCursor();
  ui->alert_text->setText("Password updated.");
  TheoreticalDiary::instance()->closeable = true;
  ui->apply_button->setEnabled(true);
  ui->ok_button->setEnabled(true);
  ui->change_password_button->setEnabled(true);
}

void OptionsMenu::download_backup() {}

void OptionsMenu::upload_diary() {}

void OptionsMenu::flush_oauth() {}

void OptionsMenu::dev_list() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
            if (td::Res::No == code) {
              return TheoreticalDiary::instance()->gwrapper->display_auth_error(
                  this);
            }

            TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

            // List all files in drive.
            connect(TheoreticalDiary::instance()->gwrapper->requestor,
                    qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                        &O2Requestor::finished),
                    [this](int id, QNetworkReply::NetworkError error,
                           QByteArray data) {
                      APIResponse r(data, this);
                      r.exec();
                    });

            TheoreticalDiary::instance()->gwrapper->list_files();
          });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_unknown_file() {
  QMessageBox rip(this);
  QPushButton ok_button("OK", &rip);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  rip.setFont(f);
  rip.setText("IO failed.");
  rip.setInformativeText("The app could not access the specified location.");
  rip.addButton(&ok_button, QMessageBox::AcceptRole);
  rip.setDefaultButton(&ok_button);
  rip.setTextInteractionFlags(Qt::NoTextInteraction);

  rip.exec();
}

void OptionsMenu::dev_upload() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
            if (td::Res::No == code) {
              return TheoreticalDiary::instance()->gwrapper->display_auth_error(
                  this);
            }

            TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

            // Upload file to drive.
            connect(TheoreticalDiary::instance()->gwrapper->requestor,
                    qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                        &O2Requestor::finished),
                    [this](int id, QNetworkReply::NetworkError error,
                           QByteArray data) {
                      APIResponse r(data, this);
                      r.exec();
                    });

            auto filename = QFileDialog::getOpenFileName(this, "Upload file",
                                                         QDir::homePath());
            if (filename.isEmpty())
              return;

            QFile f(filename);
            QFileInfo fi(f);

            auto res = TheoreticalDiary::instance()->gwrapper->upload_file(
                filename, fi.fileName());
            if (td::Res::No == res)
              dev_unknown_file();
          });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_download() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(
      TheoreticalDiary::instance()->gwrapper,
      &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
        if (td::Res::No == code) {
          return TheoreticalDiary::instance()->gwrapper->display_auth_error(
              this);
        }

        TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

        // Download file from drive.
        connect(
            TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            [this](int id, QNetworkReply::NetworkError error, QByteArray data) {
              if (QNetworkReply::NoError != error) {
                APIResponse r(data, this);
                r.exec();
                return;
              }

              auto filename = QFileDialog::getSaveFileName(
                  this, "Download file",
                  QString("%1/download").arg(QDir::homePath()));
              if (filename.isEmpty())
                return;

              QFile file(filename);
              if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
                return dev_unknown_file();
              file.write(data);
              file.close();

              QByteArray res("File downloaded.");
              APIResponse r(res, this);
              r.exec();
            });

        TheoreticalDiary::instance()->gwrapper->download_file(
            ui->dev_download_file_id->text());
      });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_update() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
            if (td::Res::No == code) {
              return TheoreticalDiary::instance()->gwrapper->display_auth_error(
                  this);
            }

            TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

            // Upload file to drive.
            connect(TheoreticalDiary::instance()->gwrapper->requestor,
                    qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                        &O2Requestor::finished),
                    [this](int id, QNetworkReply::NetworkError error,
                           QByteArray data) {
                      APIResponse r(data, this);
                      r.exec();
                    });

            auto filename = QFileDialog::getOpenFileName(this, "Update file",
                                                         QDir::homePath());
            if (filename.isEmpty())
              return;

            auto id = ui->dev_update_file_id->text();
            auto res = TheoreticalDiary::instance()->gwrapper->update_file(
                id, filename);
            if (td::Res::No == res)
              dev_unknown_file();
          });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_copy() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
            if (td::Res::No == code) {
              return TheoreticalDiary::instance()->gwrapper->display_auth_error(
                  this);
            }

            TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

            // Copy file on drive.
            connect(TheoreticalDiary::instance()->gwrapper->requestor,
                    qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                        &O2Requestor::finished),
                    [this](int id, QNetworkReply::NetworkError error,
                           QByteArray data) {
                      APIResponse r(data, this);
                      r.exec();
                    });

            TheoreticalDiary::instance()->gwrapper->copy_file(
                ui->dev_copy_file_id->text(),
                ui->dev_copy_file_new_name->text());
          });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::dev_delete() {
  TheoreticalDiary::instance()->gwrapper->dc_oauth_slots();

  // Check for OAuth2 credentials first.
  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, [this](const td::Res code) {
            if (td::Res::No == code) {
              return TheoreticalDiary::instance()->gwrapper->display_auth_error(
                  this);
            }

            TheoreticalDiary::instance()->gwrapper->dc_requestor_slots();

            // Delete file on drive.
            connect(TheoreticalDiary::instance()->gwrapper->requestor,
                    qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                        &O2Requestor::finished),
                    [this](int id, QNetworkReply::NetworkError error,
                           QByteArray data) {
                      if (QNetworkReply::NoError != error) {
                        APIResponse r(data, this);
                        r.exec();
                        return;
                      }

                      QByteArray res("File deleted.");
                      APIResponse r(res, this);
                      r.exec();
                    });

            TheoreticalDiary::instance()->gwrapper->delete_file(
                ui->dev_delete_file_id->text());
          });

  TheoreticalDiary::instance()->gwrapper->authenticate();
}

void OptionsMenu::show_about() {
  AboutDialog w(this);
  w.exec();
}

void OptionsMenu::show_licenses() {
  LicensesDialog w(this);
  w.exec();
}
