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

#include "alertlabel.h"

AlertLabel::AlertLabel(QWidget *parent) : QLabel(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    connect(m_timer, &QTimer::timeout, this, &AlertLabel::timeout);
}

void AlertLabel::set_text(const QString &&t, bool const auto_hide)
{
    if (!t.size()) {
        setVisible(false);
    }
    else {
        setText(t);
        setVisible(true);

        if (auto_hide) {
            m_timer->setInterval(5000);
            m_timer->start();
        }
        else {
            m_timer->stop();
        }
    }

    update();
}

void AlertLabel::timeout()
{
    setVisible(false);
}
