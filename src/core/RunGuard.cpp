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

#include <QCryptographicHash>

#include "RunGuard.h"

auto generate_key_hash(QString const &key, QString const &salt) -> QString
{
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}

RunGuard::RunGuard(QString const &key)
    : m_key(key), m_memLockKey(generate_key_hash(key, "1")), m_sharedMemKey(generate_key_hash(key, "2")),
      m_SharedMem(m_sharedMemKey), m_memLock(m_memLockKey, 1)
{
    m_memLock.acquire();
    {
        QSharedMemory fix(m_sharedMemKey); // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    m_memLock.release();
}

RunGuard::~RunGuard()
{
    release();
}

auto RunGuard::is_another_running() -> bool
{
    if (m_SharedMem.isAttached())
        return false;

    m_memLock.acquire();
    bool const isRunning = m_SharedMem.attach();
    if (isRunning)
        m_SharedMem.detach();
    m_memLock.release();

    return isRunning;
}

auto RunGuard::tryToRun() -> bool
{
    if (is_another_running()) // Extra check
        return false;

    m_memLock.acquire();
    bool const result = m_SharedMem.create(sizeof(quint64));
    m_memLock.release();
    if (!result) {
        release();
        return false;
    }

    return true;
}

void RunGuard::release()
{
    m_memLock.acquire();
    if (m_SharedMem.isAttached())
        m_SharedMem.detach();
    m_memLock.release();
}
