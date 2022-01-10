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

#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include <QtWidgets>

/*
 * BusyFilter class.
 */
class BusyFilter : public QObject {
protected:
    bool eventFilter(QObject *, QEvent *event);
};

/*
 * InactiveFilter class.
 */
class InactiveFilter : public QObject {
    Q_OBJECT

signals:
    void sig_inactive_timeout();

public:
    InactiveFilter(qint64 const i, QObject *parent = nullptr);
    ~InactiveFilter();

    QTimer *timer;
    qint64 interval;

public slots:
    void slot_inactive_timeout();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // EVENTFILTERS_H
