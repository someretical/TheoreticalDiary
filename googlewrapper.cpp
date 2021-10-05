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

// https://github.com/chilarai/Qt-Google-OAuth/blob/master/googleauth.cpp
GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent) {
  this->google = new QOAuth2AuthorizationCodeFlow(this);
  //  this->google->setScope("https://www.googleapis.com/auth/userinfo.profile "
  //                         "https://www.googleapis.com/auth/drive.appdata");

  //  connect(this->google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
  //          [=](QUrl url) {
  //            QUrlQuery query(url);

  //            query.addQueryItem(
  //                "prompt", "consent"); // Param required to get data
  //                everytime
  //            query.addQueryItem("access_type",
  //                               "offline"); // Needed for Refresh Token (as
  //                                           // AccessToken expires shortly)
  //            url.setQuery(query);
  //            QDesktopServices::openUrl(url);
  //          });

  //  this->google->setAuthorizationUrl(
  //      QUrl("https://accounts.google.com/o/oauth2/auth"));
  //  this->google->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
  //  this->google->setClientIdentifier(
  //      "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent."
  //      "com");
  //  this->google->setClientIdentifierSharedKey("zuyjH1Cd_8pL4Q-OFNLjNCJ7");

  //  auto reply_handler = new QOAuthHttpServerReplyHandler(5476, this);
  //  this->google->setReplyHandler(reply_handler);

  //  connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, this,
  //          &GoogleWrapper::auth_granted);
}

GoogleWrapper::~GoogleWrapper() { delete google; }

void GoogleWrapper::authenticate() { google->grant(); }

void GoogleWrapper::auth_granted() {
  qDebug() << google->token();
  qDebug() << google->refreshToken();
  qDebug() << google->scope();
  qDebug() << google->expirationAt().toString();

  QMapIterator<QString, QVariant> i(google->extraTokens());
  while (i.hasNext()) {
    i.next();
    qDebug() << i.key();
  }
}
