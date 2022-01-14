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

#ifndef HASHWORKER_H
#define HASHWORKER_H

#include "encryptor.h"

#include <QtWidgets>

/*
 * HashWorker class.
 */
class HashWorker : public QObject {
    Q_OBJECT

public slots:
    void hash(std::string const &plaintext)
    {
        Encryptor::instance()->set_key(plaintext);
        qDebug() << "Hashing done.";
        emit done();
    }

signals:
    void done();
};

/*
 * HashController class.
 */
class HashController : public QObject {
    Q_OBJECT

    QThread worker_thread;

public:
    HashController()
    {
        auto *worker = new HashWorker;
        worker->moveToThread(&worker_thread);

        connect(&worker_thread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &HashController::operate, worker, &HashWorker::hash);
        connect(worker, &HashWorker::done, this, &HashController::handle_results);

        worker_thread.start();
    }

    ~HashController()
    {
        worker_thread.quit();
        worker_thread.wait();
    }

public slots:
    void handle_results()
    {
        emit sig_done(true);
        emit sig_delete();
    }

signals:
    void operate(std::string const &plaintext);
    void sig_done(bool const);
    void sig_delete();
};

#endif // HASHWORKER_H
