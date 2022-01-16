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

#include <fstream>
#include <sstream>

#include "../util/encryptor.h"
#include "../util/zipper.h"
#include "googlewrapper.h"

char const *CLIENT_ID = "71530390003-2fr89p1c0unpd1n169munqajeepnhdco.apps.googleusercontent.com";
char const *CLIENT_SECRET = "zuyjH1Cd_8pL4Q-OFNLjNCJ7";
char const *SCOPE = "https://www.googleapis.com/auth/userinfo.profile https://www.googleapis.com/auth/drive.appdata "
                    "https://www.googleapis.com/auth/drive.file";
char const *REPLY_CONTENT =
    "<HTML><body><h1 style=\"text-align: center\">Authorization flow completed. Feel free to close "
    "this window.</h1></body></HTML>";
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
    google->setReplyContent(REPLY_CONTENT);

    QVariantMap params;
    params["access_type"] = QVariant("offline");
    google->setExtraRequestParams(params);

    // To stop o2 from writing to the disk, a dummy read only file from the resource file is provided.
    // Don't want o2 to write to disk because a custom encryption method is already employed.
    auto settings = new QSettings(":/dummysettings", QSettings::IniFormat);
    auto settings_store = new O0SettingsStore(settings, QApplication::applicationName() /* Placeholder value */);
    google->setStore(settings_store);

    manager = new QNetworkAccessManager(this);
    requestor = new O2Requestor(manager, qobject_cast<O2 *>(google), this);
    requestor->setAddAccessTokenInQuery(false);
    requestor->setAccessTokenInAuthenticationHTTPHeaderFormat("Bearer %1");

    connect(google, &O2Google::openBrowser, [](QUrl const &url) { QDesktopServices::openUrl(url); });
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
        google->setLinked(true);

        qDebug() << "Decrypted tokens.";
        return true;
    }
    catch (nlohmann::json::exception const &e) {
        qDebug() << "Exception during parsing JSON." << perform_decrypt;
        return false;
    }
}

td::LinkingResponse GoogleWrapper::verify_auth()
{
    if (!google->linked()) {
        qDebug() << "Linking failed.";
        return td::LinkingResponse::Fail;
    }

    // Confirm scopes.
    // Quite possibly the ONLY regular expression in the entire project :o
    QStringList scope_list = google->scope().split(QRegularExpression("\\+|\\s"));
    QStringList required_list = QString(SCOPE).split(" ");
    std::sort(scope_list.begin(), scope_list.end());
    std::sort(required_list.begin(), required_list.end());

    if (scope_list != required_list) {
        qDebug() << "Scopes don't match.";
        return td::LinkingResponse::ScopeMismatch;
    }
    else {
        qDebug() << "Scopes do match.";
        return td::LinkingResponse::OK;
    }
}

td::NRO GoogleWrapper::revoke_access()
{
    qDebug() << "Attempting to revoke access via Google.";
    QUrl url(QString("https://oauth2.googleapis.com/revoke?token=%1").arg(google->token()));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->post(req, "");
    return observable;
}

td::NRO GoogleWrapper::list_files()
{
    qDebug() << "Attempting to list drive files.";
    QUrl url("https://www.googleapis.com/drive/v3/files?spaces=appDataFolder");
    QNetworkRequest req(url);

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->get(req);
    return observable;
}

// file is a pointer to a DYNAMICALLY allocated QFile object!
td::NRO GoogleWrapper::upload_file(QFile *file, QString const &name)
{
    qDebug() << "Attempting to upload file:" << name;
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

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    // The content type and content length headers are automatically set by Qt.
    requestor->post(req, multi_part);
    return observable;
}

td::NRO GoogleWrapper::copy_file(QString const &id, QString const &new_name)
{
    qDebug() << "Attempting to copy file:" << id << new_name;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1/copy").arg(id));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->post(req, QString("{\"name\":\"%1\",\"parents\":[\"appDataFolder\"]}").arg(new_name).toUtf8());
    return observable;
}

td::NRO GoogleWrapper::download_file(QString const &id)
{
    qDebug() << "Attempting to download file:" << id;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1?alt=media").arg(id));
    QNetworkRequest req(url);

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->get(req);
    return observable;
}

td::NRO GoogleWrapper::delete_file(QString const &id)
{
    qDebug() << "Attempting to delete file:" << id;
    QUrl url(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
    QNetworkRequest req(url);

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->deleteResource(req);
    return observable;
}

// file is a pointer to a DYNAMICALLY allocated QFile object!
td::NRO GoogleWrapper::update_file(QFile *file, QString const &id)
{
    qDebug() << "Attempting to update file:" << id;

    QUrl url(QString("https://www.googleapis.com/upload/drive/v3/files/%1?uploadType=multipart").arg(id));
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

    auto observable = AsyncFuture::observe(requestor, qOverload<td::NR>(&O2Requestor::finished));
    requestor->customRequest(req, "PATCH", multi_part);
    return observable;
}

std::pair<QString, QString> GoogleWrapper::get_file_ids(QByteArray const &data)
{
    auto primary_backup_id = QString();
    auto secondary_backup_id = QString();

    auto const &json = nlohmann::json::parse(data.toStdString(), nullptr, false);
    if (json.is_discarded()) {
        qDebug() << "Failed to parse drive file IDs:" << data;
        return std::make_pair(primary_backup_id, secondary_backup_id);
    }

    for (auto const &file : json["files"]) {
        if ("drive#file" == file["kind"]) {
            if ("diary.dat" == file["name"]) {
                primary_backup_id = QString::fromStdString(file["id"]);
            }
            else if ("diary.dat.bak" == file["name"]) {
                secondary_backup_id = QString::fromStdString(file["id"]);
            }
        }
    }

    qDebug() << "Parsed drive file IDs:" << primary_backup_id << secondary_backup_id;
    return std::make_pair(primary_backup_id, secondary_backup_id);
}
