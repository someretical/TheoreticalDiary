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

#include "RunGuard.h"

QString generate_key_hash(QString const &key, QString const &salt)
{
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}

RunGuard::RunGuard(QString const &key)
    : m_key(key), m_mem_lock_key(generate_key_hash(key, QStringLiteral("_mem_lock_key"))),
      m_shared_mem_key(generate_key_hash(key, QStringLiteral("_shared_mem_key"))), m_shared_mem(m_shared_mem_key),
      m_mem_lock(m_mem_lock_key, 1)
{
    m_mem_lock.acquire();
    {
        QSharedMemory fix(m_shared_mem_key); // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    m_mem_lock.release();
}

RunGuard::~RunGuard()
{
    release();
}

bool RunGuard::is_another_running()
{
    if (m_shared_mem.isAttached())
        return false;

    m_mem_lock.acquire();
    bool const isRunning = m_shared_mem.attach();
    if (isRunning)
        m_shared_mem.detach();
    m_mem_lock.release();

    return isRunning;
}

bool RunGuard::try_to_run()
{
    if (is_another_running()) // Extra check.
        return false;

    m_mem_lock.acquire();
    bool const result = m_shared_mem.create(sizeof(quint64));
    m_mem_lock.release();
    if (!result) {
        release();
        return false;
    }

    return true;
}

void RunGuard::release()
{
    m_mem_lock.acquire();
    if (m_shared_mem.isAttached())
        m_shared_mem.detach();
    m_mem_lock.release();
}
