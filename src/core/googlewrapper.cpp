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
#include <QJsonDocument>
#include <QJsonObject>
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
  google->blockSignals(true);
  google->setClientId(CLIENT_ID);
  google->setClientSecret(CLIENT_SECRET);
  google->setScope(SCOPE);
  google->setLocalPort(PORT);

  auto settings = new QSettings(
      QString("%1/%2").arg(TheoreticalDiary::instance()->data_location(),
                           "credentials.ini"),
      QSettings::IniFormat);
  auto settings_store = new O0SettingsStore(
      settings, QApplication::applicationName() /* This is NOT secure!!! */);
  google->setStore(settings_store);

  manager = new QNetworkAccessManager(this);
  requester = new O2Requestor(manager, google, this);

  connect(google, &O2Google::linkingSucceeded, this, &GoogleWrapper::auth_ok);
  connect(google, &O2Google::linkingFailed, this, &GoogleWrapper::auth_err);
  connect(google, &O2Google::openBrowser, this, &GoogleWrapper::open_browser);

  // These changes should be committed
  connect(google, &O2Google::refreshTokenUrlChanged, p,
          &TheoreticalDiary::oauth_changed);
  connect(google, &O2Google::tokenChanged, p, &TheoreticalDiary::oauth_changed);
}

GoogleWrapper::~GoogleWrapper() {
  delete google;
  delete manager;
  delete requester;
}

void GoogleWrapper::open_browser(const QUrl &url) {
  auto res = QDesktopServices::openUrl(url);

  if (!res) {
    google->blockSignals(true);

    emit sig_oauth2_callback(td::Res::No);
  }
}

void GoogleWrapper::authenticate() {
  google->blockSignals(false);

  google->link();
}

void GoogleWrapper::auth_ok() {
  google->blockSignals(true);

  emit sig_oauth2_callback(td::Res::Yes);
}

void GoogleWrapper::auth_err() {
  google->blockSignals(true);

  emit sig_oauth2_callback(td::Res::No);
}

//
//
// Drive downloader

// DriveDownloader::DriveDownloader(const QString &path, const QString &file_id,
//                                 QObject *parent) {
//  dest = new QFile(path);

//  QVariantMap params;
//  params.insert("spaces", "appDataFolder");
//  params.insert("alt", "media");
//  reply = TheoreticalDiary::instance()->gwrapper->google->get(
//      QUrl("https://www.googleapis.com/drive/v3/files/" + file_id), params);

//  connect(reply, &QNetworkReply::downloadProgress, this,
//          &DriveDownloader::download_progress);
//  connect(reply, &QNetworkReply::finished, this,
//          &DriveDownloader::download_finished);
//  connect(reply, &QNetworkReply::readyRead, this,
//          &DriveDownloader::packet_received);

//  if (!dest->open(QIODevice::WriteOnly)) {
//    qDebug() << "can't write";
//    emit finished(td::Res::No);
//  }
//}

// DriveDownloader::~DriveDownloader() {
//  delete reply;
//  delete dest;
//}

// void DriveDownloader::download_finished() {
//  if (QNetworkReply::NoError != reply->error()) {
//    qDebug() << "network err2" << reply->error();
//    emit finished(td::Res::No);
//    return;
//  }

//  dest->close();
//  emit finished(td::Res::Yes);
//  qDebug() << "finished downloading file";
//}

// void DriveDownloader::download_progress(const qint64 bytesRead,
//                                        const qint64 totalBytes) {
//  qDebug() << "downloaded" << bytesRead << "/" << totalBytes
//           << (bytesRead / totalBytes * 100) << "%";
//}

// void DriveDownloader::packet_received() {
//  // readAll reads the content of the buffer and then flushes the buffer
//  dest->write(reply->readAll());
//}
