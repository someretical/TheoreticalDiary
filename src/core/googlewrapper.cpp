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

#include "googlewrapper.h"
#include "o0settingsstore.h"
#include "theoreticaldiary.h"

#include <json.hpp>

const char *CLIENT_ID =
    "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent.com";
const char *CLIENT_SECRET = "zuyjH1Cd_8pL4Q-OFNLjNCJ7";
const char *SCOPE = "https://www.googleapis.com/auth/userinfo.profile "
                    "https://www.googleapis.com/auth/drive.appdata";
const int PORT = 8888;

GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent) {
  google = new O2Google(this);
  google->setClientId(CLIENT_ID);
  google->setClientSecret(CLIENT_SECRET);
  google->setScope(SCOPE);
  google->setLocalPort(PORT);

  primary_backup_id = new QString("");
  secondary_backup_id = new QString("");
  silent_upload_diary = false;

  QVariantMap params;
  params["access_type"] = QVariant("offline");
  google->setExtraRequestParams(params);

  auto settings = new QSettings(
      QString("%1/%2").arg(TheoreticalDiary::instance()->data_location(),
                           "config.ini"),
      QSettings::IniFormat);
  auto settings_store = new O0SettingsStore(
      settings, QApplication::applicationName() /* This is NOT secure!!! */);
  google->setStore(settings_store);

  manager = new QNetworkAccessManager(this);
  requestor = new O2Requestor(manager, qobject_cast<O2 *>(google), this);
  requestor->setAddAccessTokenInQuery(false);
  requestor->setAccessTokenInAuthenticationHTTPHeaderFormat("Bearer %1");

  connect(google, &O2Google::linkingSucceeded, this, &GoogleWrapper::auth_ok,
          Qt::QueuedConnection);
  connect(google, &O2Google::linkingFailed, this, &GoogleWrapper::auth_err,
          Qt::QueuedConnection);
  connect(google, &O2Google::openBrowser, this, &GoogleWrapper::open_browser,
          Qt::QueuedConnection);
}

GoogleWrapper::~GoogleWrapper() {
  delete google;
  delete manager;
  delete requestor;
  delete primary_backup_id;
  delete secondary_backup_id;
}

void GoogleWrapper::open_browser(const QUrl &url) {
  if (!QDesktopServices::openUrl(url))
    emit sig_oauth2_callback(td::Res::No);
}

void GoogleWrapper::authenticate() { google->link(); }

void GoogleWrapper::auth_ok() {
  QStringList scope_list = google->scope().split(" ");
  QStringList required_list = QString(SCOPE).split(" ");
  std::sort(scope_list.begin(), scope_list.end());
  std::sort(required_list.begin(), required_list.end());

  if (scope_list != required_list)
    emit sig_oauth2_callback(td::Res::No);
  else
    emit sig_oauth2_callback(td::Res::Yes);
}

void GoogleWrapper::auth_err() { emit sig_oauth2_callback(td::Res::No); }

void GoogleWrapper::dc_oauth_slots() {
  disconnect(this, &GoogleWrapper::sig_oauth2_callback, nullptr, nullptr);
}

void GoogleWrapper::dc_requestor_slots() {
  disconnect(requestor,
             qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                 &O2Requestor::finished),
             nullptr, nullptr);
  disconnect(requestor, &O2Requestor::uploadProgress, nullptr, nullptr);
}

void GoogleWrapper::list_files() {
  QUrl url("https://www.googleapis.com/drive/v3/files?spaces=appDataFolder");
  QNetworkRequest req(url);
  requestor->get(req);
}

td::Res GoogleWrapper::upload_file(const QString &local_path,
                                   const QString &name) {
  QFile *file = new QFile(local_path);
  if (!file->open(QIODevice::ReadOnly))
    return td::Res::No;

  QUrl url(
      "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart");
  QNetworkRequest req(url);
  auto multi_part = new QHttpMultiPart(QHttpMultiPart::RelatedType);

  QHttpPart metadata;
  metadata.setHeader(QNetworkRequest::ContentTypeHeader,
                     "application/json; charset=UTF-8");
  metadata.setBody(QString("{\"name\":\"%1\",\"parents\":[\"appDataFolder\"]}")
                       .arg(name)
                       .toUtf8());

  QHttpPart media;
  media.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/octet-stream");
  media.setBodyDevice(file);
  // When multi_part is deleted, file will be deleted as well.
  file->setParent(multi_part);

  multi_part->append(metadata);
  multi_part->append(media);

  // The content type and content length headers are automatically set by Qt.
  TheoreticalDiary::instance()->gwrapper->requestor->post(req, multi_part);

  return td::Res::Yes;
}

void GoogleWrapper::copy_file(const QString &id, const QString &new_name) {
  QUrl url(
      QString("https://www.googleapis.com/drive/v3/files/%1/copy").arg(id));
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  requestor->post(req,
                  QString("{\"name\":\"%1\",\"parents\":[\"appDataFolder\"]}")
                      .arg(new_name)
                      .toUtf8());
}

void GoogleWrapper::download_file(const QString &id) {
  QUrl url(QString("https://www.googleapis.com/drive/v3/files/"
                   "%1?alt=media")
               .arg(id));
  QNetworkRequest req(url);
  requestor->get(req);
}

void GoogleWrapper::delete_file(const QString &id) {
  QUrl url(QString("https://www.googleapis.com/drive/v3/files/"
                   "%1")
               .arg(id));
  QNetworkRequest req(url);
  requestor->deleteResource(req);
}

td::Res GoogleWrapper::update_file(const QString &id,
                                   const QString &local_path) {
  QFile *file = new QFile(local_path);
  if (!file->open(QIODevice::ReadOnly))
    return td::Res::No;

  QUrl url(QString("https://www.googleapis.com/upload/drive/v3/files/"
                   "%1?uploadType=multipart")
               .arg(id));
  QNetworkRequest req(url);
  auto multi_part = new QHttpMultiPart(QHttpMultiPart::RelatedType);

  QHttpPart metadata;
  metadata.setHeader(QNetworkRequest::ContentTypeHeader,
                     "application/json; charset=UTF-8");
  metadata.setBody("{}");

  QHttpPart media;
  media.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/octet-stream");
  media.setBodyDevice(file);
  file->setParent(multi_part);

  multi_part->append(metadata);
  multi_part->append(media);

  TheoreticalDiary::instance()->gwrapper->requestor->customRequest(req, "PATCH",
                                                                   multi_part);

  return td::Res::Yes;
}

void GoogleWrapper::display_auth_error(QWidget *p) {
  QMessageBox rip(p);
  QPushButton ok_button("OK", &rip);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  rip.setFont(f);
  rip.setText("Authentication error.");
  rip.setInformativeText("The app was unable to authenticate with Google.");
  rip.addButton(&ok_button, QMessageBox::AcceptRole);
  rip.setDefaultButton(&ok_button);
  rip.setTextInteractionFlags(Qt::NoTextInteraction);

  rip.exec();
}

void GoogleWrapper::display_network_error(QWidget *p) {
  QMessageBox rip(p);
  QPushButton ok_button("OK", &rip);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  rip.setFont(f);
  rip.setText("Network error.");
  rip.setInformativeText("The app encountered a network error.");
  rip.addButton(&ok_button, QMessageBox::AcceptRole);
  rip.setDefaultButton(&ok_button);
  rip.setTextInteractionFlags(Qt::NoTextInteraction);

  rip.exec();
}

void GoogleWrapper::get_file_ids(const QByteArray &data) {
  *primary_backup_id = "";
  *secondary_backup_id = "";

  auto json = nlohmann::json::parse(data.toStdString(), nullptr, false);
  if (json.is_discarded())
    return;

  for (const auto &file : json["files"]) {
    if ("drive#file" == file["kind"] &&
        "application/octet-stream" == file["mimeType"]) {
      if ("diary.dat" == file["name"]) {
        *primary_backup_id = QString::fromStdString(file["id"]);
      } else if ("diary.dat.bak" == file["name"]) {
        *secondary_backup_id = QString::fromStdString(file["id"]);
      }
    }
  }
}

void GoogleWrapper::download_diary(QWidget *p) {
  dc_oauth_slots();
  current_dialog_parent = p;

  /*
   * Download process:
   * - Get OAuth2 credentials.
   * - [Google Drive] Fetch list of files in the appDataFolder.
   * - Store the ids of the diary and its backup.
   * - [Google Drive] Try to download the primary backup if it exists.
   * - [Google Drive] Otherwise download the secondary backup if it exists.
   * - Save the download to a temporary file.
   * - Once the download is complete, copy the contents of the tempoary file to
   * the local diary location.
   * - Delete the temporary file.
   * - Inform the user that the download was successful.
   */

  connect(this, &GoogleWrapper::sig_oauth2_callback, [&](const td::Res code) {
    if (td::Res::No == code) {
      emit sig_request_end();
      return display_auth_error(current_dialog_parent);
    }

    dc_requestor_slots();
    connect(requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::download__list_files_cb);
    list_files();
  });
  authenticate();
}

void GoogleWrapper::download__list_files_cb(
    const int, const QNetworkReply::NetworkError error, const QByteArray data) {
  if (QNetworkReply::NoError != error) {
    emit sig_request_end();
    return display_network_error(current_dialog_parent);
  }

  get_file_ids(data);

  if (primary_backup_id->isEmpty() && secondary_backup_id->isEmpty()) {
    QMessageBox rip(current_dialog_parent);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
    rip.setText("No backups found.");
    rip.setInformativeText("No backups could be found on Google Drive.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    emit sig_request_end();
    rip.exec();
    return;
  }

  const QString id =
      primary_backup_id->isEmpty() ? *secondary_backup_id : *primary_backup_id;

  dc_requestor_slots();
  connect(requestor,
          qOverload<int, QNetworkReply::NetworkError, QByteArray>(
              &O2Requestor::finished),
          this, &GoogleWrapper::download__download_file_cb);
  download_file(id);
}

void GoogleWrapper::download__download_file_cb(
    const int, const QNetworkReply::NetworkError error, const QByteArray data) {
  if (QNetworkReply::NoError != error) {
    emit sig_request_end();
    return display_network_error(current_dialog_parent);
  }

  QFile file(QString("%1/diary.dat")
                 .arg(TheoreticalDiary::instance()->data_location()));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QMessageBox rip(current_dialog_parent);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
    rip.setText("Download failed.");
    rip.setInformativeText("The app could not write to the diary location.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    emit sig_request_end();
    rip.exec();
    return;
  }
  // At the moment, the data is written all in one go as
  // o2 does not yet provide any download updates. This is
  // really bad practice so TODO: add own implementation
  // of a download notifier so the data buffer can be
  // piped into the file every time a new chunk arrives.
  file.write(data);
  file.close();

  QMessageBox ok(current_dialog_parent);
  QPushButton ok_button("OK", &ok);
  ok_button.setFlat(true);
  QFont f = ok_button.font();
  f.setPointSize(11);
  ok_button.setFont(f);

  ok.setFont(f);
  ok.setText("Diary downloaded.");
  ok.setInformativeText("The diary has been downloaded from Google Drive.");
  ok.addButton(&ok_button, QMessageBox::AcceptRole);
  ok.setDefaultButton(&ok_button);
  ok.setTextInteractionFlags(Qt::NoTextInteraction);

  emit sig_request_end();
  ok.exec();
}

void GoogleWrapper::upload_diary(QWidget *p, const bool silent) {
  dc_oauth_slots();
  current_dialog_parent = p;
  silent_upload_diary = silent;

  /*
   * Upload process:
   * - Get OAuth2 credentials.
   * - [Google Drive] Fetch list of files in the appDataFolder
   * - [Google Drive] Create a copy of the primary backup file.
   * - [Google Drive] Delete the old secondary backup.
   * - Check local diary exists.
   * - [Google Drive] Patch the contents of the primary backup file with the
   * local diary file.
   * - Inform the user that the upload was successful.
   */

  connect(this, &GoogleWrapper::sig_oauth2_callback, [&](const td::Res code) {
    if (td::Res::No == code) {
      emit sig_request_end();
      return display_auth_error(current_dialog_parent);
    }

    dc_requestor_slots();
    connect(requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::upload__list_files_cb);
    list_files();
  });
  authenticate();
}

void GoogleWrapper::upload__list_files_cb(
    const int, const QNetworkReply::NetworkError error, const QByteArray data) {
  if (QNetworkReply::NoError != error) {
    emit sig_request_end();
    return display_network_error(current_dialog_parent);
  }

  get_file_ids(data);
  dc_requestor_slots();
  if (primary_backup_id->isEmpty()) {
    connect(requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::upload__upload_file_cb);

    const auto res = TheoreticalDiary::instance()->gwrapper->upload_file(
        QString("%1/diary.dat")
            .arg(TheoreticalDiary::instance()->data_location()),
        QString("diary.dat"));
    if (td::Res::No == res) {
      QMessageBox rip(current_dialog_parent);
      QPushButton ok_button("OK", &rip);
      ok_button.setFlat(true);
      QFont f = ok_button.font();
      f.setPointSize(11);
      ok_button.setFont(f);

      rip.setFont(f);
      rip.setText("Read error.");
      rip.setInformativeText(
          "The app was unable to read the contents of the diary.");
      rip.addButton(&ok_button, QMessageBox::AcceptRole);
      rip.setDefaultButton(&ok_button);
      rip.setTextInteractionFlags(Qt::NoTextInteraction);

      emit sig_request_end();
      rip.exec();
    }
  } else {
    connect(requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::upload__copy_file_cb);
    copy_file(*primary_backup_id, QString("diary.dat.bak"));
  }
}

void GoogleWrapper::upload__copy_file_cb(
    const int, const QNetworkReply::NetworkError error, const QByteArray) {
  if (QNetworkReply::NoError != error) {
    emit sig_request_end();
    return display_network_error(current_dialog_parent);
  }

  dc_requestor_slots();
  if (secondary_backup_id->isEmpty()) {
    connect(TheoreticalDiary::instance()->gwrapper->requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::upload__upload_file_cb);

    const auto name = QString("%1/diary.dat")
                          .arg(TheoreticalDiary::instance()->data_location());
    const auto res = update_file(*primary_backup_id, name);
    if (td::Res::No == res) {
      QMessageBox rip(current_dialog_parent);
      QPushButton ok_button("OK", &rip);
      ok_button.setFlat(true);
      QFont f = ok_button.font();
      f.setPointSize(11);
      ok_button.setFont(f);

      rip.setFont(f);
      rip.setText("Read error.");
      rip.setInformativeText(
          "The app was unable to read the contents of the diary.");
      rip.addButton(&ok_button, QMessageBox::AcceptRole);
      rip.setDefaultButton(&ok_button);
      rip.setTextInteractionFlags(Qt::NoTextInteraction);

      emit sig_request_end();
      rip.exec();
    }
  } else {
    connect(requestor,
            qOverload<int, QNetworkReply::NetworkError, QByteArray>(
                &O2Requestor::finished),
            this, &GoogleWrapper::upload__delete_file_cb);
    delete_file(*secondary_backup_id);
  }
}

void GoogleWrapper::upload__delete_file_cb(
    const int, const QNetworkReply::NetworkError error, QByteArray) {
  if (QNetworkReply::NoError != error) {
    emit sig_request_end();
    return display_network_error(current_dialog_parent);
  }

  dc_requestor_slots();
  connect(requestor,
          qOverload<int, QNetworkReply::NetworkError, QByteArray>(
              &O2Requestor::finished),
          this, &GoogleWrapper::upload__upload_file_cb);

  const auto name = QString("%1/diary.dat")
                        .arg(TheoreticalDiary::instance()->data_location());
  const auto res = update_file(*primary_backup_id, name);
  if (td::Res::No == res) {
    QMessageBox rip(current_dialog_parent);
    QPushButton ok_button("OK", &rip);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    rip.setFont(f);
    rip.setText("Read error.");
    rip.setInformativeText(
        "The app was unable to read the contents of the diary.");
    rip.addButton(&ok_button, QMessageBox::AcceptRole);
    rip.setDefaultButton(&ok_button);
    rip.setTextInteractionFlags(Qt::NoTextInteraction);

    emit sig_request_end();
    rip.exec();
  }
}

void GoogleWrapper::upload__upload_file_cb(
    const int, const QNetworkReply::NetworkError error, const QByteArray) {
  emit sig_request_end();

  if (QNetworkReply::NoError != error)
    return display_network_error(current_dialog_parent);

  if (!silent_upload_diary) {
    QMessageBox ok(current_dialog_parent);
    QPushButton ok_button("OK", &ok);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    ok.setFont(f);
    ok.setText("Diary uploaded.");
    ok.setInformativeText("The diary has been uploaded to Google Drive.");
    ok.addButton(&ok_button, QMessageBox::AcceptRole);
    ok.setDefaultButton(&ok_button);
    ok.setTextInteractionFlags(Qt::NoTextInteraction);

    ok.exec();
  }

  qDebug() << "Uploaded diary to Google Drive.";
}
