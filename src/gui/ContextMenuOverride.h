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

#ifndef THEORETICAL_DIARY_CONTEXTMENUOVERRIDE_H
#define THEORETICAL_DIARY_CONTEXTMENUOVERRIDE_H

#include "core/Util.h"
#include "gui/Icons.h"

#include <QContextMenuEvent>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMenu>
#include <QPlainTextEdit>
#include <QSpinBox>

class LineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        if (QMenu *menu = createStandardContextMenu()) {
            overrideStandardContextMenuIcons(menu);
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->popup(event->globalPos());
        }
    }
};

/* =============================================================================================== */

class PlainTextEdit : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit PlainTextEdit(QWidget *parent = nullptr) : QPlainTextEdit(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        if (QMenu *menu = createStandardContextMenu()) {
            overrideStandardContextMenuIcons(menu);
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->popup(event->globalPos());
        }
    }
};

/* =============================================================================================== */

class DoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

public:
    explicit DoubleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        spinBoxContextMenuOverrideEvent(event, this, lineEdit(), stepEnabled());
    }
};

/* =============================================================================================== */

class SpinBox : public QSpinBox {
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        spinBoxContextMenuOverrideEvent(event, this, lineEdit(), stepEnabled());
    }
};

#endif // THEORETICAL_DIARY_CONTEXTMENUOVERRIDE_H
