/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class RunGuard {

public:
  RunGuard(const QString &key);
  ~RunGuard();

  bool isAnotherRunning();
  bool tryToRun();
  void release();

private:
  const QString key;
  const QString memLockKey;
  const QString sharedmemKey;

  QSharedMemory sharedMem;
  QSystemSemaphore memLock;

  Q_DISABLE_COPY(RunGuard)
};

#endif // RUNGUARD_H