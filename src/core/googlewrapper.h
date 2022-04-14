/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
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

#include <QtWidgets>
#include <utility>

#include "asyncfuture.h"
#include "internalmanager.h"
#include "o0settingsstore.h"
#include "o2google.h"
#include "o2requestor.h"

// These have to be 'pre-declared' as extern if any template functions want to use them.
// See https://stackoverflow.com/a/2328715
extern char const *CLIENT_ID;
extern char const *CLIENT_SECRET;
extern char const *SCOPE;
extern char const *REPLY_CONTENT;
extern int const PORT;

class GoogleWrapper : public QObject {
    Q_OBJECT

signals:
    void sig_oauth2_callback(bool const success);

public:
    GoogleWrapper(QObject *parent = nullptr);
    ~GoogleWrapper();
    static GoogleWrapper *instance();

    O2Google *m_o2g;
    QNetworkAccessManager *m_man;
    O2Requestor *m_req;

    bool encrypt_credentials();
    bool decrypt_credentials(bool const perform_decrypt);
    td::LinkingResponse verify_auth();
    td::NRO list_files();
    td::NRO upload_file(QFile *file, QString const &name);
    td::NRO copy_file(QString const &id, QString const &new_name);
    td::NRO download_file(QString const &id);
    td::NRO delete_file(QString const &id);
    td::NRO update_file(QFile *file, QString const &id);
    td::NRO revoke_access();
    std::pair<QString, QString> get_file_ids(QByteArray const &data);
};

#endif // GOOGLEWRAPPER_H
