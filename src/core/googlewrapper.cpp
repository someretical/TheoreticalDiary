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

char const *CLIENT_ID = "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent.com";
char const *CLIENT_SECRET = "zuyjH1Cd_8pL4Q-OFNLjNCJ7";
char const *SCOPE = "https://www.googleapis.com/auth/userinfo.profile "
                    "https://www.googleapis.com/auth/drive.appdata";
int const PORT = 8888;
GoogleWrapper *google_wrapper_ptr;

GoogleWrapper::GoogleWrapper(QObject *parent) : QObject(parent)
{
    google_wrapper_ptr = this;
    google = new O2Google(this);
    google->setClientId(CLIENT_ID);
    google->setClientSecret(CLIENT_SECRET);
    google->setScope(SCOPE);
    google->setLocalPort(PORT);

    primary_backup_id = QString();
    secondary_backup_id = QString();

    QVariantMap params;
    params["access_type"] = QVariant("offline");
    google->setExtraRequestParams(params);

    // To stop o2 from writing to the disk, a dummy real only file from the resource file is provided.
    // Don't want o2 to write to disk because a custom encryption method is already employed.
    auto settings = new QSettings(":/dummysettings", QSettings::IniFormat);
    auto settings_store = new O0SettingsStore(settings, QApplication::applicationName() /* Placeholder value */);
    google->setStore(settings_store);

    manager = new QNetworkAccessManager(this);
    requestor = new O2Requestor(manager, qobject_cast<O2 *>(google), this);
    requestor->setAddAccessTokenInQuery(false);
    requestor->setAccessTokenInAuthenticationHTTPHeaderFormat("Bearer %1");

    connect(google, &O2Google::linkingSucceeded, this, &GoogleWrapper::auth_ok, Qt::QueuedConnection);
    connect(google, &O2Google::linkingFailed, this, &GoogleWrapper::auth_err, Qt::QueuedConnection);
    connect(google, &O2Google::openBrowser, this, &GoogleWrapper::open_browser, Qt::QueuedConnection);
}

GoogleWrapper::~GoogleWrapper()
{
    delete google;
    delete manager;
    delete requestor;
}

GoogleWrapper *GoogleWrapper::instance()
{
    return google_wrapper_ptr;
}

void GoogleWrapper::open_browser(QUrl const &url)
{
    qDebug() << "Attempting to open browser for authentication.";

    if (!QDesktopServices::openUrl(url)) {
        emit sig_oauth2_callback(false);
        qDebug() << "Failed to open browser for authentication.";
    }
}

void GoogleWrapper::authenticate()
{
    qDebug() << "Attempting to link Google.";
    google->link();
}

bool GoogleWrapper::encrypt_credentials()
{
    nlohmann::json const j = td::Credentials{google->token().toStdString(), google->refreshToken().toStdString()};

    // Gzip JSON.
    std::string compressed, encrypted, decompressed = j.dump();
    Zipper::zip(compressed, decompressed);

    // Encrypt if there is a password set.
    auto const key_set = Encryptor::instance()->key_set;
    if (key_set)
        Encryptor::instance()->encrypt(compressed, encrypted);

    qDebug() << (key_set ? "Encrypted tokens." : "No encryption key set for tokens.");

    // Write to file.
    std::ofstream ofs(
        InternalManager::instance()->data_location().toStdString() + "/credentials.secret", std::ios::binary);
    if (!ofs.fail()) {
        ofs << (key_set ? encrypted : compressed);
        qDebug() << "Saved tokens.";
        return true;
    }

    qDebug() << "Failed to write encrypted tokens to disk.";
    return false;
}

bool GoogleWrapper::decrypt_credentials(bool const perform_decrypt)
{
    std::ifstream ifs(InternalManager::instance()->data_location().toStdString() + "/credentials.secret");
    if (ifs.fail()) {
        qDebug() << "Couldn't find encrypted token file. perform_decrypt is " << perform_decrypt;
        return false;
    }

    std::stringstream stream;
    stream << ifs.rdbuf();
    std::string decrypted, str = stream.str();

    if (perform_decrypt) {
        if (str.size() <= tencrypt::SALT_SIZE + tencrypt::IV_SIZE) {
            qDebug() << "Token file found but way too short to be decrypted.";
            return false;
        }
        else {
            Encryptor::instance()->parse_encrypted_string(str);
            auto const &res = Encryptor::instance()->decrypt(str);
            if (!res) {
                qDebug() << "Bad token decrypt.";
                return false;
            }

            decrypted.assign(*res);
        }
    }

    std::string decompressed;
    if (!Zipper::unzip((perform_decrypt ? decrypted : str), decompressed)) {
        qDebug() << "Couldn't decompress the tokens." << perform_decrypt;
        return false;
    }

    auto const &json = nlohmann::json::parse(decompressed, nullptr, false);
    if (json.is_discarded()) {
        qDebug() << "Invalid token JSON" << perform_decrypt;
        return false;
    }

    try {
        auto credentials = json.get<td::Credentials>();
        google->setToken(credentials.access_token.data());
        google->setRefreshToken(credentials.refresh_token.data());

        qDebug() << "Decrypted tokens.";
        return true;
    }
    catch (nlohmann::json::exception const &e) {
        qDebug() << "Exception during parsing JSON." << perform_decrypt;
        return false;
    }
}

void GoogleWrapper::auth_ok()
{
    // Quite possibly the ONLY regular expression in the entire project :o
    QStringList scope_list = google->scope().split(QRegularExpression("\\+|\\s"));
    QStringList required_list = QString(SCOPE).split(" ");
    std::sort(scope_list.begin(), scope_list.end());
    std::sort(required_list.begin(), required_list.end());

    if (scope_list != required_list) {
        qDebug() << "Scopes don't match.";
        emit sig_oauth2_callback(false);
    }
    else {
        qDebug() << "Scopes do match.";
        emit sig_oauth2_callback(true);
    }
}

void GoogleWrapper::auth_err()
{
    qDebug() << "Authentication error.";
    emit sig_oauth2_callback(false);
}

void GoogleWrapper::revoke_access()
{
    QUrl url(QString("https://oauth2.googleapis.com/revoke?token=%1").arg(google->token()));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    requestor->post(req, "");
    qDebug() << "Attempted to revoke access via Google.";
}

void GoogleWrapper::dc_oauth_slots()
{
    disconnect(this, &GoogleWrapper::sig_oauth2_callback, nullptr, nullptr);
    qDebug() << "Disconnected all oauth slots.";
}

void GoogleWrapper::dc_requestor_slots()
{
    disconnect(
        requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), nullptr, nullptr);
    disconnect(requestor, &O2Requestor::uploadProgress, nullptr, nullptr);
    qDebug() << "Disconnected all requestor slots.";
}

void GoogleWrapper::list_files()
{
    QUrl url("https://www.googleapis.com/drive/v3/files?spaces=appDataFolder");
    QNetworkRequest req(url);
    requestor->get(req);
    qDebug() << "Attempted to list drive files.";
}

bool GoogleWrapper::upload_file(QString const &local_path, QString const &name)
{
    qDebug() << "Uploading file" << local_path << name;
    QFile *file = new QFile(local_path);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't find local file to upload:" << local_path;
        return false;
    }

    QUrl url("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart");
    QNetworkRequest req(url);
    auto multi_part = new QHttpMultiPart(QHttpMultiPart::RelatedType);

    QHttpPart metadata;
    metadata.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    metadata.setBody(QString("{\"name\":\"%1\",\"parents\":[\"appDataFolder\"]}").arg(name).toUtf8());

    QHttpPart media;
    media.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    media.setBodyDevice(file);
    // When multi_part is deleted, file will be deleted as well.
    file->setParent(multi_part);

    multi_part->append(metadata);
    multi_part->append(media);

    // The content type and content length headers are automatically set by Qt.
    requestor->post(req, multi_part);

    return true;
}

void GoogleWrapper::copy_file(QString const &id, QString const &new_name)
{
    qDebug() << "Copying file:" << id << new_name;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1/copy").arg(id));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    requestor->post(req, QString("{\"name\":\"%1\",\"parents\":[\"appDataFolder\"]}").arg(new_name).toUtf8());
}

void GoogleWrapper::download_file(QString const &id)
{
    qDebug() << "Downloading file:" << id;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/"
                     "%1?alt=media")
                 .arg(id));
    QNetworkRequest req(url);
    requestor->get(req);
}

void GoogleWrapper::delete_file(QString const &id)
{
    qDebug() << "Deleting file:" << id;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/"
                     "%1")
                 .arg(id));
    QNetworkRequest req(url);
    requestor->deleteResource(req);
}

bool GoogleWrapper::update_file(QString const &id, QString const &local_path)
{
    qDebug() << "Updating file:" << id << local_path;
    QFile *file = new QFile(local_path);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't find local file to update:" << local_path;
        return false;
    }

    QUrl url(QString("https://www.googleapis.com/upload/drive/v3/files/"
                     "%1?uploadType=multipart")
                 .arg(id));
    QNetworkRequest req(url);
    auto multi_part = new QHttpMultiPart(QHttpMultiPart::RelatedType);

    QHttpPart metadata;
    metadata.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    metadata.setBody("{}");

    QHttpPart media;
    media.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    media.setBodyDevice(file);
    file->setParent(multi_part);

    multi_part->append(metadata);
    multi_part->append(media);

    requestor->customRequest(req, "PATCH", multi_part);

    return true;
}

void GoogleWrapper::get_file_ids(QByteArray const &data)
{
    primary_backup_id = QString();
    secondary_backup_id = QString();

    auto const &json = nlohmann::json::parse(data.toStdString(), nullptr, false);
    if (json.is_discarded()) {
        qDebug() << "Failed to parse drive file IDs:" << data;
        return;
    }

    for (auto const &file : json["files"]) {
        if ("drive#file" == file["kind"] && "application/octet-stream" == file["mimeType"]) {
            if ("diary.dat" == file["name"]) {
                primary_backup_id = QString::fromStdString(file["id"]);
            }
            else if ("diary.dat.bak" == file["name"]) {
                secondary_backup_id = QString::fromStdString(file["id"]);
            }
        }
    }

    qDebug() << "Parsed drive file IDs:" << primary_backup_id << secondary_backup_id;
}

void GoogleWrapper::download_diary(final_cb_t const &final_cb)
{
    qDebug() << "Downloading diary...";
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    final_callback = final_cb;
    dc_oauth_slots();

    /*
     * Download process:
     * - Get OAuth2 credentials.
     * - [Google Drive] Fetch list of files in the appDataFolder.
     * - Store the ids of the diary and its backup.
     * - [Google Drive] Try to download the primary backup if it exists.
     * - [Google Drive] Otherwise download the secondary backup if it exists.
     * - Save the download to a temporary file.
     * - Once the download is complete, copy the contents of the tempoary file to
     * the local diary location.
     * - Delete the temporary file.
     * - Inform the user that the download was successful.
     */

    connect(this, &GoogleWrapper::sig_oauth2_callback, [&](bool const success) {
        dc_oauth_slots();

        if (!success)
            return final_callback(td::GWrapperError::Auth);

        dc_requestor_slots();
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::download__list_files_cb);
        list_files();
    });
    authenticate();
}

void GoogleWrapper::download__list_files_cb(int const, QNetworkReply::NetworkError const error, QByteArray const data)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    if (QNetworkReply::NoError != error)
        return final_callback(td::GWrapperError::Network);

    get_file_ids(data);

    if (primary_backup_id.isEmpty() && secondary_backup_id.isEmpty()) {
        final_callback(td::GWrapperError::DriveFile);
    }
    else {
        auto const &id = primary_backup_id.isEmpty() ? secondary_backup_id : primary_backup_id;

        dc_requestor_slots();
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::download__download_file_cb);
        download_file(id);
    }
}

void GoogleWrapper::download__download_file_cb(
    int const, QNetworkReply::NetworkError const error, QByteArray const data)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    if (QNetworkReply::NoError != error)
        return final_callback(td::GWrapperError::Network);

    QFile file(QString("%1/diary.dat").arg(InternalManager::instance()->data_location()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        final_callback(td::GWrapperError::IO);
    }
    else {
        // At the moment, the data is written all in one go as o2 does not yet provide any download updates. This is
        // really bad practice so TODO: add own implementation of a download notifier so the data buffer can be
        // piped into the file every time a new chunk arrives.
        file.write(data);

        final_callback(td::GWrapperError::None);
    }
}

void GoogleWrapper::upload_diary(final_cb_t const &final_cb)
{
    qDebug() << "Uploading diary...";
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);
    final_callback = final_cb;
    dc_oauth_slots();

    /*
     * Upload process:
     * - Get OAuth2 credentials.
     * - [Google Drive] Fetch list of files in the appDataFolder
     * - [Google Drive] Create a copy of the primary backup file.
     * - [Google Drive] Delete the old secondary backup.
     * - Check local diary exists.
     * - [Google Drive] Patch the contents of the primary backup file with the
     * local diary file.
     * - Inform the user that the upload was successful.
     */

    connect(this, &GoogleWrapper::sig_oauth2_callback, [&](bool const success) {
        dc_oauth_slots();

        if (!success)
            return final_callback(td::GWrapperError::Auth);

        dc_requestor_slots();
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::upload__list_files_cb);
        list_files();
    });
    authenticate();
}

void GoogleWrapper::upload__list_files_cb(int const, QNetworkReply::NetworkError const error, QByteArray const data)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    if (QNetworkReply::NoError != error)
        return final_callback(td::GWrapperError::Network);

    get_file_ids(data);
    dc_requestor_slots();
    if (primary_backup_id.isEmpty()) {
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::upload__upload_file_cb);

        auto const success = upload_file(
            QString("%1/diary.dat").arg(InternalManager::instance()->data_location()), QString("diary.dat"));
        if (!success)
            return final_callback(td::GWrapperError::IO);
    }
    else {
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::upload__copy_file_cb);
        copy_file(primary_backup_id, QString("diary.dat.bak"));
    }
}

void GoogleWrapper::upload__copy_file_cb(int const, QNetworkReply::NetworkError const error, QByteArray const)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    if (QNetworkReply::NoError != error)
        return final_callback(td::GWrapperError::Network);

    dc_requestor_slots();
    if (secondary_backup_id.isEmpty()) {
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::upload__upload_file_cb);

        auto const &name = QString("%1/diary.dat").arg(InternalManager::instance()->data_location());
        auto const success = update_file(primary_backup_id, name);
        if (!success)
            return final_callback(td::GWrapperError::IO);
    }
    else {
        connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
            &GoogleWrapper::upload__delete_file_cb);
        delete_file(secondary_backup_id);
    }
}

void GoogleWrapper::upload__delete_file_cb(int const, QNetworkReply::NetworkError const error, QByteArray const)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    if (QNetworkReply::NoError != error)
        return final_callback(td::GWrapperError::Network);

    dc_requestor_slots();
    connect(requestor, qOverload<int, QNetworkReply::NetworkError, QByteArray>(&O2Requestor::finished), this,
        &GoogleWrapper::upload__upload_file_cb);

    auto const &name = QString("%1/diary.dat").arg(InternalManager::instance()->data_location());
    auto const success = update_file(primary_backup_id, name);
    if (!success)
        return final_callback(td::GWrapperError::IO);
}

void GoogleWrapper::upload__upload_file_cb(int const, QNetworkReply::NetworkError const error, QByteArray const)
{
    qDebug() << "Flow passed through cb" << __LINE__ << __func__ << __FILE__;
    final_callback(QNetworkReply::NoError != error ? td::GWrapperError::Network : td::GWrapperError::None);
}
