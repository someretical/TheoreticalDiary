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

#ifndef GOOGLEWRAPPER_H
#define GOOGLEWRAPPER_H

#include <QObject>
#include <QtNetworkAuth>

class GoogleWrapper : public QObject {
  Q_OBJECT

public:
  GoogleWrapper(QObject *parent = nullptr);
  ~GoogleWrapper();
  void authenticate();

  // The members below SHOULD be private. The caveats are outlined in the
  // comments.

  // This function is needed by PrompAuth
  void auth_err();
  // *google is needed by MainWindow::closeEvent
  QOAuth2AuthorizationCodeFlow *google;

signals:
  void sig_auth_err();
  void sig_auth_ok();
  void sig_token_changed();

private:
  void token_changed();
  void auth_ok();
};

#endif // GOOGLEWRAPPER_H
