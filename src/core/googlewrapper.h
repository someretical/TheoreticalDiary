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

#ifndef GOOGLEWRAPPER_H
#define GOOGLEWRAPPER_H

// Foward declaration.
namespace td {
enum Res : int;
}

#include "o2google.h"
#include "o2requestor.h"
#include "theoreticaldiary.h"

#include <QtWidgets>

class GoogleWrapper : public QObject {
    Q_OBJECT

signals:
    void sig_oauth2_callback(td::Res const code);
    void sig_request_end();

public:
    GoogleWrapper(QObject *parent = nullptr);
    ~GoogleWrapper();
    void authenticate();

    void list_files();
    td::Res upload_file(QString const &local_path, QString const &name);
    void copy_file(QString const &id, QString const &new_name);
    void download_file(QString const &id);
    void delete_file(QString const &id);
    void revoke_access();
    td::Res update_file(QString const &id, QString const &local_path);

    O2Google *google;
    QNetworkAccessManager *manager;
    O2Requestor *requestor;

private:
    void get_file_ids(QByteArray const &data);

    // These exist as members here because it's too annoying to pass them through 4 callbacks.
    bool silent_upload_diary;
    QString primary_backup_id;
    QString secondary_backup_id;
    QWidget *current_dialog_parent;

public slots:
    void auth_err();
    void dc_oauth_slots();
    void dc_requestor_slots();
    void display_auth_error(QWidget *p);
    void display_network_error(QWidget *p);
    void display_read_error(QWidget *p);
    void download_diary(QWidget *p);
    void download__list_files_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void download__download_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload_diary(QWidget *p, bool const silent);
    void upload__list_files_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload__copy_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload__delete_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray data);
    void upload__upload_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);

private slots:
    void auth_ok();
    void open_browser(QUrl const &url);
};

#endif // GOOGLEWRAPPER_H
