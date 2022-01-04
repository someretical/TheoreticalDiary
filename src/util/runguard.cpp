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

#include "runguard.h"

QString generate_key_hash(QString const &key, QString const &salt)
{
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}

RunGuard::RunGuard(QString const &key)
    : key(key), mem_lock_key(generate_key_hash(key, "_mem_lock_key")),
      shared_mem_key(generate_key_hash(key, "_shared_mem_key")), shared_mem(shared_mem_key), mem_lock(mem_lock_key, 1)
{
    mem_lock.acquire();
    {
        QSharedMemory fix(shared_mem_key); // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    mem_lock.release();
}

RunGuard::~RunGuard()
{
    release();
}

bool RunGuard::is_another_running()
{
    if (shared_mem.isAttached())
        return false;

    mem_lock.acquire();
    bool const isRunning = shared_mem.attach();
    if (isRunning)
        shared_mem.detach();
    mem_lock.release();

    return isRunning;
}

bool RunGuard::try_to_run()
{
    if (is_another_running()) // Extra check.
        return false;

    mem_lock.acquire();
    bool const result = shared_mem.create(sizeof(quint64));
    mem_lock.release();
    if (!result) {
        release();
        return false;
    }

    return true;
}

void RunGuard::release()
{
    mem_lock.acquire();
    if (shared_mem.isAttached())
        shared_mem.detach();
    mem_lock.release();
}
