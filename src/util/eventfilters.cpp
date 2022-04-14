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

#include "eventfilters.h"

/*
 * BusyFilter class.
 */
bool BusyFilter::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::DragEnter:
    case QEvent::DragLeave:
    case QEvent::DragMove:
    case QEvent::Drop:
    case QEvent::GraphicsSceneDragMove:
    case QEvent::GraphicsSceneDrop:
    case QEvent::GraphicsSceneHelp:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneWheel:
    case QEvent::InputMethod:
    case QEvent::InputMethodQuery:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Shortcut:
    case QEvent::ShortcutOverride:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::TouchBegin:
    case QEvent::TouchCancel:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
        return true;
    default:
        return false;
    }
}

/*
 * InactiveFilter class.
 * Thanks to https://www.qtcentre.org/threads/1464-Detecting-user-inaction
 */
InactiveFilter::InactiveFilter(qint64 const i, QObject *parent) : QObject(parent)
{
    m_interval = i;
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    m_timer->start(m_interval);
    connect(m_timer, &QTimer::timeout, this, &InactiveFilter::slot_inactive_timeout);
}

InactiveFilter::~InactiveFilter()
{
    delete m_timer;
}

bool InactiveFilter::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::DragEnter:
    case QEvent::DragLeave:
    case QEvent::DragMove:
    case QEvent::Drop:
    case QEvent::GraphicsSceneDragMove:
    case QEvent::GraphicsSceneDrop:
    case QEvent::GraphicsSceneHelp:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneWheel:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Shortcut:
    case QEvent::ShortcutOverride:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::TouchBegin:
    case QEvent::TouchCancel:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
        m_timer->start(m_interval);
        break;
    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

void InactiveFilter::slot_inactive_timeout()
{
    if (0 != m_interval)
        emit sig_inactive_timeout();
}
