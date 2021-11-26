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

#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

const char *CLIENT_ID =
    "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent.com";
const char *CLIENT_SECRET = "zuyjH1Cd_8pL4Q-OFNLjNCJ7";
const char *SCOPE = "https://www.googleapis.com/auth/userinfo.profile "
                    "https://www.googleapis.com/auth/drive.appdata";
const int PORT = 8888;

GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent) {
  auto p = qobject_cast<TheoreticalDiary *>(parent);
  google = new O2Google(this);
  google->setClientId(CLIENT_ID);
  google->setClientSecret(CLIENT_SECRET);
  google->setScope(SCOPE);
  google->setLocalPort(PORT);

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

  // These changes should be committed
  connect(google, &O2Google::refreshTokenUrlChanged, p,
          &TheoreticalDiary::oauth_changed, Qt::QueuedConnection);
  connect(google, &O2Google::tokenChanged, p, &TheoreticalDiary::oauth_changed,
          Qt::QueuedConnection);
}

GoogleWrapper::~GoogleWrapper() {
  delete google;
  delete manager;
  delete requestor;
}

void GoogleWrapper::open_browser(const QUrl &url) {
  if (!QDesktopServices::openUrl(url))
    emit sig_oauth2_callback(td::Res::No);
}

void GoogleWrapper::authenticate() { google->link(); }

void GoogleWrapper::auth_ok() { emit sig_oauth2_callback(td::Res::Yes); }

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

td::Res GoogleWrapper::update_file(QString &id, QString &local_path) {
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
