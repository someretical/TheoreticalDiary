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

#include "../gui/mainwindow.h"
#include "../util/custommessageboxes.h"
#include "../util/encryptor.h"
#include "../util/zipper.h"
#include "internalmanager.h"
#include "o0settingsstore.h"
#include "o2google.h"
#include "o2requestor.h"

#include <QtWidgets>
#include <fstream>
#include <functional>
#include <json.hpp>
#include <sstream>
#include <string>

class GoogleWrapper : public QObject {
    Q_OBJECT

signals:
    void sig_oauth2_callback(bool const success);

public:
    GoogleWrapper(QObject *parent = nullptr);
    ~GoogleWrapper();
    static GoogleWrapper *instance();

    void authenticate();
    bool encrypt_credentials();                           // ***
    bool decrypt_credentials(bool const perform_decrypt); // ***

    void list_files();                                                // ***
    bool upload_file(QString const &local_path, QString const &name); // ***
    void copy_file(QString const &id, QString const &new_name);       // ***
    void download_file(QString const &id);                            // ***
    void delete_file(QString const &id);                              // ***
    bool update_file(QString const &id, QString const &local_path);   // ***
    void revoke_access();

    // *** It is the responsibility of the caller to start and end busy mode!

    O2Google *google;
    QNetworkAccessManager *manager;
    O2Requestor *requestor;

private:
    void get_file_ids(QByteArray const &data);

    // Temporary variables that store Google Drive IDs.
    QString primary_backup_id;
    QString secondary_backup_id;

    typedef std::function<void /* return type */ (td::GWrapperError)> final_cb_t;
    // This will hold a lambda as a callback. It will be executed whenever download_diary and upload_diary are
    // completed.
    final_cb_t final_callback;

public slots:
    void auth_err();
    void dc_oauth_slots();
    void dc_requestor_slots();
    void download_diary(final_cb_t const &final_cb);
    void upload_diary(final_cb_t const &final_cb);

private slots:
    void auth_ok();
    void open_browser(QUrl const &url);

    void download__list_files_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void download__download_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload__list_files_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload__copy_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
    void upload__delete_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray data);
    void upload__upload_file_cb(int const id, QNetworkReply::NetworkError const error, QByteArray const data);
};

#endif // GOOGLEWRAPPER_H
