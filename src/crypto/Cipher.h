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

#ifndef THEORETICAL_DIARY_CIPHER_H
#define THEORETICAL_DIARY_CIPHER_H

#include "botan_all.h"

#include <QString>
#include <QScopedPointer>
#include <QByteArray>

class Cipher
{
public:
    explicit Cipher() = default;
    ~Cipher() = default;

    auto initialised() const -> bool;
    auto init(Botan::Cipher_Dir direction, const QByteArray& key, const QByteArray& iv) -> bool;
    auto process(char* data, int len) -> bool;
    auto process(QByteArray& data) -> bool;
    auto finish(QByteArray& data) -> bool;

    void reset();

    auto errorString() const -> QString;

private:
    QString m_error;
    QScopedPointer<Botan::Cipher_Mode> m_cipher;

    Q_DISABLE_COPY(Cipher)
};

#endif // THEORETICAL_DIARY_CIPHER_H
