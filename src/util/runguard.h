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

#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QtWidgets>

class RunGuard {
public:
    RunGuard(QString const &m_key);
    ~RunGuard();

    bool is_another_running();
    bool try_to_run();
    void release();

private:
    QString const m_key;
    QString const m_mem_lock_key;
    QString const m_shared_mem_key;

    QSharedMemory m_shared_mem;
    QSystemSemaphore m_mem_lock;

    Q_DISABLE_COPY(RunGuard)
};

#endif // RUNGUARD_H
