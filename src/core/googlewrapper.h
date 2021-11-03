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

#ifndef GOOGLEWRAPPER_H
#define GOOGLEWRAPPER_H

// Foward declaration
namespace td {
enum Res : int;
}

#include "o2google.h"
#include "o2requestor.h"
#include "theoreticaldiary.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>

class GoogleWrapper : public QObject {
  Q_OBJECT

signals:
  void sig_oauth2_callback(const td::Res code);

public:
  GoogleWrapper(QObject *parent = nullptr);
  ~GoogleWrapper();
  void authenticate();
  //  bool load_credentials();
  //  bool save_credentials();

  O2Google *google;
  QNetworkAccessManager *manager;
  O2Requestor *requester;

public slots:
  void auth_err();

private slots:
  void auth_ok();
  void open_browser(const QUrl &url);
};

// class DriveDownloader : public QObject {
//   Q_OBJECT

// signals:
//   void update_progress(qint64 bytesRead, qint64 totalBytes);
//   void finished(const td::Res code);

// public:
//   DriveDownloader(const QString &path, const QString &file_id,
//                   QObject *parent = nullptr);
//   ~DriveDownloader();

//   QNetworkReply *reply;
//   QFile *dest;

// public slots:
//   void download_progress(const qint64 bytesRead, const qint64 totalBytes);
//   void packet_received();
//   void download_finished();
// };

#endif // GOOGLEWRAPPER_H
