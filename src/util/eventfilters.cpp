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

#include "eventfilters.h"

bool BusyFilter::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    // Fall through.
    case QEvent::DragEnter:
    // Fall through.
    case QEvent::DragLeave:
    // Fall through.
    case QEvent::DragMove:
    // Fall through.
    case QEvent::Drop:
    // Fall through.
    case QEvent::GraphicsSceneDragMove:
    // Fall through.
    case QEvent::GraphicsSceneDrop:
    // Fall through.
    case QEvent::GraphicsSceneHelp:
    // Fall through.
    case QEvent::GraphicsSceneMouseDoubleClick:
    // Fall through.
    case QEvent::GraphicsSceneMousePress:
    // Fall through.
    case QEvent::GraphicsSceneMouseRelease:
    // Fall through.
    case QEvent::GraphicsSceneWheel:
    // Fall through.
    case QEvent::InputMethod:
    // Fall through.
    case QEvent::InputMethodQuery:
    // Fall through.
    case QEvent::KeyPress:
    // Fall through.
    case QEvent::KeyRelease:
    // Fall through.
    case QEvent::MouseButtonDblClick:
    // Fall through.
    case QEvent::MouseButtonPress:
    // Fall through.
    case QEvent::MouseButtonRelease:
    // Fall through.
    case QEvent::Shortcut:
    // Fall through.
    case QEvent::ShortcutOverride:
    // Fall through.
    case QEvent::TabletPress:
    // Fall through.
    case QEvent::TabletRelease:
    // Fall through.
    case QEvent::TouchBegin:
    // Fall through.
    case QEvent::TouchCancel:
    // Fall through.
    case QEvent::TouchEnd:
    // Fall through.
    case QEvent::TouchUpdate:
    // Fall through.
    case QEvent::Wheel:
        return true;
    default:
        return false;
    }
}
