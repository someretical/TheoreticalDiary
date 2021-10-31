/**
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

#include "googlewrapper.h"
#include "mainwindow.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QtNetwork>

// Adapted from
// https://github.com/chilarai/Qt-Google-OAuth/blob/master/googleauth.cpp
GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent) {
  expires_at = new QDate(QDate::currentDate());
  contains_valid_info = new bool(false);
  google = new QOAuth2AuthorizationCodeFlow(this);
  google->blockSignals(true);

  google->setAuthorizationUrl(
      QUrl("https://accounts.google.com/o/oauth2/auth"));
  google->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
  google->setClientIdentifier(
      "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent."
      "com");
  google->setClientIdentifierSharedKey("zuyjH1Cd_8pL4Q-OFNLjNCJ7");
  google->setScope("https://www.googleapis.com/auth/userinfo.profile "
                   "https://www.googleapis.com/auth/drive.appdata");

  // https://stackoverflow.com/a/63311694
  google->setModifyParametersFunction(
      [](QAbstractOAuth::Stage stage, QVariantMap *parameters) {
        switch (stage) {
        case QAbstractOAuth::Stage::RequestingAccessToken: {
          QByteArray code = parameters->value("code").toByteArray();

          (*parameters)["code"] = QUrl::fromPercentEncoding(code);
          break;
        }
        case QAbstractOAuth::Stage::RefreshingAccessToken: {
          break;
        }
        };
      });

  connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
          [=](QUrl url) {
            QUrlQuery query(url);

            query.addQueryItem("prompt", "consent");
            query.addQueryItem("access_type", "offline");

            url.setQuery(query);
            QDesktopServices::openUrl(url);
          });

  connect(google, &QOAuth2AuthorizationCodeFlow::granted, this,
          &GoogleWrapper::auth_ok);
  connect(google, &QOAuth2AuthorizationCodeFlow::error, this,
          &GoogleWrapper::auth_err);

  // These changes should be committed to persistent storage
  connect(google, &QOAuth2AuthorizationCodeFlow::tokenChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::changes_made);
  connect(google, &QOAuth2AuthorizationCodeFlow::refreshTokenChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::changes_made);
  connect(google, &QOAuth2AuthorizationCodeFlow::expirationAtChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::changes_made);

  auto rh = new QOAuthHttpServerReplyHandler(8888, this);
  google->setReplyHandler(rh);
}

GoogleWrapper::~GoogleWrapper() {
  delete google;
  delete contains_valid_info;
  delete expires_at;
}

bool GoogleWrapper::load_credentials() {
  QFile file(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      "/TheoreticalDiary/credentials.json");

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString data = file.readAll();
    file.close();
    QJsonDocument text = QJsonDocument::fromJson(data.toUtf8());

    if (!text.isNull()) {
      auto tokens = text.object();

      if (!tokens.contains("access_token") ||
          !tokens.contains("refresh_token") || !tokens.contains("expires_at"))
        return false;

      google->setToken(tokens["access_token"].toString());
      google->setRefreshToken(tokens["refresh_token"].toString());
      *contains_valid_info = true;
      *expires_at =
          QDateTime::fromMSecsSinceEpoch(tokens["expires_at"].toDouble())
              .date();

      return true;
    }
  }

  return false;
}

bool GoogleWrapper::save_credentials() {
  // Both tokens need to exist. Only having one is useless.
  if (google->token().isEmpty() || google->refreshToken().isEmpty() ||
      !*contains_valid_info)
    return false;

  QJsonObject tokens;
  QJsonDocument doc;

  tokens.insert("access_token", google->token());
  tokens.insert("refresh_token", google->refreshToken());
  tokens.insert("expires_at", google->expirationAt().currentMSecsSinceEpoch());
  doc.setObject(tokens);

  QFile file(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      "/TheoreticalDiary/credentials.json");

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(doc.toJson());
    file.close();

    return true;
  } else {
    return false;
  }
}

void GoogleWrapper::authenticate() {
  google->blockSignals(false);

  if (load_credentials()) {
    emit sig_oauth2_callback(td::Res::Yes);
  } else {
    google->grant();
  }
}

void GoogleWrapper::auth_ok() {
  QStringList scope_list = google->scope().split(" ");
  QStringList required_list = {
      "https://www.googleapis.com/auth/drive.appdata",
      "https://www.googleapis.com/auth/userinfo.profile"};
  std::sort(scope_list.begin(), scope_list.end());

  if (scope_list != required_list)
    return auth_err();

  *contains_valid_info = true;

  if (save_credentials()) {
    google->blockSignals(true);
    emit sig_oauth2_callback(td::Res::Yes);
  } else {
    auth_err();
  }
}

void GoogleWrapper::auth_err() {
  google->blockSignals(true);
  *contains_valid_info = false;

  emit sig_oauth2_callback(td::Res::No);
}

//
//
// Drive downloader

DriveDownloader::DriveDownloader(const QString &path, const QString &file_id,
                                 QObject *parent) {
  dest = new QFile(path);

  QVariantMap params;
  params.insert("spaces", "appDataFolder");
  params.insert("alt", "media");
  reply = TheoreticalDiary::instance()->gwrapper->google->get(
      QUrl("https://www.googleapis.com/drive/v3/files/" + file_id), params);

  connect(reply, &QNetworkReply::downloadProgress, this,
          &DriveDownloader::download_progress);
  connect(reply, &QNetworkReply::finished, this,
          &DriveDownloader::download_finished);
  connect(reply, &QNetworkReply::readyRead, this,
          &DriveDownloader::packet_received);

  if (!dest->open(QIODevice::WriteOnly)) {
    qDebug() << "can't write";
    emit finished(td::Res::No);
  }
}

DriveDownloader::~DriveDownloader() {
  delete reply;
  delete dest;
}

void DriveDownloader::download_finished() {
  if (QNetworkReply::NoError != reply->error()) {
    qDebug() << "network err2" << reply->error();
    emit finished(td::Res::No);
    return;
  }

  dest->close();
  emit finished(td::Res::Yes);
  qDebug() << "finished downloading file";
}

void DriveDownloader::download_progress(const qint64 bytesRead,
                                        const qint64 totalBytes) {
  qDebug() << "downloaded" << bytesRead << "/" << totalBytes
           << (bytesRead / totalBytes * 100) << "%";
}

void DriveDownloader::packet_received() {
  // readAll reads the content of the buffer and then flushes the buffer
  dest->write(reply->readAll());
}
