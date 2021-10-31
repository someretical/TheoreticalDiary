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

#ifndef GOOGLEWRAPPER_H
#define GOOGLEWRAPPER_H

namespace td {
enum Res : int;
}

#include "theoreticaldiary.h"

#include <QDate>
#include <QFile>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QtNetworkAuth>

class GoogleWrapper : public QObject {
  Q_OBJECT

public:
  GoogleWrapper(QObject *parent = nullptr);
  ~GoogleWrapper();
  void authenticate();
  bool save_credentials();
  bool load_credentials();
  void auth_err();

  bool *contains_valid_info;
  QOAuth2AuthorizationCodeFlow *google;
  QDate *expires_at;

signals:
  void sig_oauth2_callback(const td::Res code);

private:
  void auth_ok();
};

class DriveDownloader : public QObject {
  Q_OBJECT

signals:
  void update_progress(qint64 bytesRead, qint64 totalBytes);
  void finished(const td::Res code);

public:
  DriveDownloader(const QString &path, const QString &file_id,
                  QObject *parent = nullptr);
  ~DriveDownloader();

  QNetworkReply *reply;
  QFile *dest;

public slots:
  void download_progress(const qint64 bytesRead, const qint64 totalBytes);
  void packet_received();
  void download_finished();
};

#endif // GOOGLEWRAPPER_H
