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

#include "googlewrapper.h"
#include "mainwindow.h"
#include "theoreticaldiary.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QtNetwork>
#include <QtNetworkAuth>

// https://github.com/chilarai/Qt-Google-OAuth/blob/master/googleauth.cpp
GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent) {
  google = new QOAuth2AuthorizationCodeFlow(this);

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
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
          QByteArray code = parameters->value("code").toByteArray();
          (*parameters)["code"] = QUrl::fromPercentEncoding(code);
        }
      });

  connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
          [=](QUrl url) {
            QUrlQuery query(url);

            query.addQueryItem("prompt",
                               "consent"); // Param required to get data
            query.addQueryItem("access_type",
                               "offline"); // Needed for Refresh Token (as
                                           // AccessToken expires shortly)
            url.setQuery(query);
            QDesktopServices::openUrl(url);
          });

  auto reply_handler = new QOAuthHttpServerReplyHandler(5476, this);
  google->setReplyHandler(reply_handler);

  connect(google, &QOAuth2AuthorizationCodeFlow::granted, this,
          &GoogleWrapper::auth_ok);
  connect(google, &QOAuth2AuthorizationCodeFlow::error, this,
          &GoogleWrapper::auth_err);

  // These changes should be committed to persistent storage
  connect(google, &QOAuth2AuthorizationCodeFlow::tokenChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::changes_made);
  connect(google, &QOAuth2AuthorizationCodeFlow::refreshTokenChanged,
          TheoreticalDiary::instance(), &TheoreticalDiary::changes_made);
}

GoogleWrapper::~GoogleWrapper() { delete google; }

void GoogleWrapper::authenticate() {
  QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
             "/credentials.json");

  if (file.open(QIODevice::ReadOnly)) {
    QString data = file.readAll();
    file.close();
    QJsonDocument text = QJsonDocument::fromJson(data.toUtf8());

    if (!text.isNull()) {
      auto tokens = text.object();

      if (tokens.contains("access_token") && tokens.contains("refresh_token")) {
        google->setToken(tokens["access_token"].toString());
        google->setRefreshToken(tokens["refresh_token"].toString());

        emit sig_auth_ok();
        return;
      }
    }
  }

  google->grant();
}

void GoogleWrapper::auth_ok() {
  QJsonObject tokens;
  QJsonDocument doc;

  QStringList scope_list = google->scope().split(" ");
  QStringList required_list = {
      "https://www.googleapis.com/auth/drive.appdata",
      "https://www.googleapis.com/auth/userinfo.profile"};
  std::sort(scope_list.begin(), scope_list.end());

  if (scope_list != required_list)
    return auth_err();

  tokens.insert("access_token", google->token());
  tokens.insert("refresh_token", google->refreshToken());
  doc.setObject(tokens);

  QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
             "/credentials.json");

  if (file.open(QIODevice::ReadWrite)) {
    file.write(doc.toJson());
    file.close();

    emit sig_auth_ok();
  } else {
    auth_err();
  }
}

void GoogleWrapper::auth_err() { emit sig_auth_err(); }
