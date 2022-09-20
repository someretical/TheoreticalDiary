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

#include "core/Util.h"
#include "gui/Icons.h"

#include <QDir>


/**
 * Get the application data path used to store configuration files
 *
 * @return
 */
auto dataPath() -> QString
{
#ifdef QT_DEBUG
    return QStringLiteral("%1/.theoreticaldiary/debug").arg(QDir::homePath());
#else
    return QString("%1/.theoreticaldiary").arg(QDir::homePath());
#endif
}

/**
 * Get the ordinal suffix for a number. E.g. -st, -nd, -rd, -th
 *
 * @param number
 * @return
 */
auto getOrdinalSuffix(int const number) -> QString
{
    switch (number) {
    case 1:
    case 21:
    case 31:
        return "st";
    case 2:
    case 22:
        return "nd";
    case 3:
    case 23:
        return "rd";
    default:
        return "th";
    }
}

/**
 * The default Qt standard context menu sets icons using QIcon::FromTheme which doesn't recolour the icons meaning they
 * don't display properly. Here, we load the correct icons from the custom Icons class.
 *
 * This function is meant to be called within contextMenuEvent
 *
 * @param menu The context menu created by createStandardContextMenu
 */
void overrideStandardContextMenuIcons(QMenu *menu)
{
    for (auto action : menu->actions())
        action->setIcon(icons().icon(action->icon().name()));
}

/**
 * Refactored method for overriding the context menu of spinbox widgets. This is so messy because Qt does not expose an
 * API to override that specific context menu meaning this code is basically copy pasted from the QAbstractSpinBox
 * source.
 *
 * @param event
 * @param ptr
 * @param lineEdit
 * @param stepEnabled
 */
void spinBoxContextMenuOverrideEvent(
    QContextMenuEvent *event, QAbstractSpinBox *ptr, QLineEdit *lineEdit, QAbstractSpinBox::StepEnabled stepEnabled)
{
    if (QPointer<QMenu> menu = lineEdit->createStandardContextMenu()) {
        overrideStandardContextMenuIcons(menu);
        const auto selAll = menu->actions().last();
        selAll->setShortcut(QKeySequence::SelectAll);
        menu->addSeparator();

        QAction *up = menu->addAction(QAbstractSpinBox::tr("&Step up"));
        up->setEnabled(stepEnabled & ptr->StepUpEnabled);
        QAction *down = menu->addAction(QAbstractSpinBox::tr("Step &down"));
        down->setEnabled(stepEnabled & ptr->StepDownEnabled);
        menu->addSeparator();

        const QPointer<QAbstractSpinBox> that = ptr;
        const QPoint pos =
            (event->reason() == QContextMenuEvent::Mouse)
                ? event->globalPos()
                : ptr->mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(ptr->width() / 2, ptr->height() / 2);
        const QAction *action = menu->exec(pos);
        delete static_cast<QMenu *>(menu);
        if (that && action) {
            if (action == up) {
                ptr->stepBy(1);
            }
            else if (action == down) {
                ptr->stepBy(-1);
            }
            else if (action == selAll) {
                ptr->selectAll();
            }
        }
        event->accept();
    }
}