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

#include "Cipher.h"

auto Cipher::init(Botan::Cipher_Dir direction, const QByteArray& key, const QByteArray& iv) -> bool
{
    try {
        auto cipher = Botan::Cipher_Mode::create_or_throw("AES-256/GCM", direction);
        m_cipher.reset(cipher.release());
        m_cipher->set_key(reinterpret_cast<const uint8_t*>(key.data()), key.size());
        m_cipher->start(reinterpret_cast<const uint8_t*>(iv.data()), iv.size());
    } catch (std::exception& e) {
        m_cipher.reset();
        m_error = e.what();
        reset();
        return false;
    }

    return true;
}

auto Cipher::initialised() const -> bool
{
    return m_cipher;
}

auto Cipher::process(char* data, int len) -> bool
{
    try {
        // Block size is checked by Botan, an exception is thrown if invalid
        m_cipher->process(reinterpret_cast<uint8_t*>(data), len);
        return true;
    } catch (std::exception& e) {
        m_error = e.what();
        return false;
    }
}

auto Cipher::process(QByteArray& data) -> bool
{
    return process(data.data(), data.size());
}

auto Cipher::finish(QByteArray& data) -> bool
{
    try {
        // Error checking is done by Botan, an exception is thrown if invalid
        Botan::secure_vector<uint8_t> input(data.begin(), data.end());
        m_cipher->finish(input);
        // Post-finished data may be larger than before due to padding
        data.resize(input.size());
        // Direct copy the finished data back into the QByteArray
        std::copy(input.begin(), input.end(), data.begin());
        return true;
    } catch (std::exception& e) {
        m_error = e.what();
        return false;
    }
}

void Cipher::reset()
{
    m_error.clear();
    if (initialised())
        m_cipher.reset();

}

auto Cipher::errorString() const -> QString
{
    return m_error;
}
