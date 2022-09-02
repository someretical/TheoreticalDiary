/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Logger.h>

#include "Hash.h"

Hash::Hash(Hash::Algorithm algo, bool hmac)
{

    switch (algo) {
    case Hash::Sha256:
        if (hmac) {
            m_hmacFunction.reset(Botan::MessageAuthenticationCode::create("HMAC(SHA-256)").release());
        }
        else {
            m_hashFunction.reset(Botan::HashFunction::create("SHA-256").release());
        }
        break;
    case Hash::Sha512:
        if (hmac) {
            m_hmacFunction.reset(Botan::MessageAuthenticationCode::create("HMAC(SHA-512)").release());
        }
        else {
            m_hashFunction.reset(Botan::HashFunction::create("SHA-512").release());
        }
        break;
    }
}

Hash::~Hash() = default;

void Hash::addData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return;
    }

    try {
        if (m_hmacFunction) {
            m_hmacFunction->update(reinterpret_cast<const uint8_t *>(data.data()), data.size());
        }
        else if (m_hashFunction) {
            m_hashFunction->update(reinterpret_cast<const uint8_t *>(data.data()), data.size());
        }
    }
    catch (std::exception &e) {
        LOG_ERROR() << "hmac/hash function update failed to add data:" << e.what();
    }
}

auto Hash::result() const -> QByteArray
{
    Botan::secure_vector<uint8_t> result;

    if (m_hmacFunction) {
        result = m_hmacFunction->final();
    }
    else if (m_hashFunction) {
        result = m_hashFunction->final();
    }

    return QByteArray(reinterpret_cast<const char *>(result.data()), result.size());
}

void Hash::setKey(const QByteArray &data)
{
    if (m_hmacFunction) {
        try {
            m_hmacFunction->set_key(reinterpret_cast<const uint8_t *>(data.data()), data.size());
        }
        catch (std::exception &e) {
            LOG_ERROR() << "setKey failed to set HMAC key:", e.what();
        }
    }
}

auto Hash::hash(const QByteArray &data, Hash::Algorithm algo) -> QByteArray
{
    Hash cryptoHash(algo);
    cryptoHash.addData(data);
    return cryptoHash.result();
}

auto Hash::hmac(const QByteArray &data, const QByteArray &key, Hash::Algorithm algo) -> QByteArray
{
    Hash cryptoHash(algo, true);
    cryptoHash.setKey(key);
    cryptoHash.addData(data);
    return cryptoHash.result();
}
