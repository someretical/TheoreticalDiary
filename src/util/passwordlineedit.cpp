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

// The code in this file was adapted from https://stackoverflow.com/a/51194796.

#include "passwordlineedit.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setEchoMode(QLineEdit::Password);
    QAction *action = addAction(QIcon(get_eye_icon(false)), QLineEdit::TrailingPosition);
    button = qobject_cast<QToolButton *>(action->associatedWidgets().last());
    button->setCursor(QCursor(Qt::PointingHandCursor));
    connect(button, &QToolButton::pressed, this, &PasswordLineEdit::on_pressed, Qt::QueuedConnection);
    connect(button, &QToolButton::released, this, &PasswordLineEdit::on_released, Qt::QueuedConnection);
}

void PasswordLineEdit::on_pressed()
{
    QToolButton *button = qobject_cast<QToolButton *>(sender());
    button->setIcon(QIcon(get_eye_icon(true)));
    setEchoMode(QLineEdit::Normal);
}

void PasswordLineEdit::on_released()
{
    QToolButton *button = qobject_cast<QToolButton *>(sender());
    button->setIcon(QIcon(get_eye_icon(false)));
    setEchoMode(QLineEdit::Password);
}

QString PasswordLineEdit::get_eye_icon(bool const on)
{
    return QString(":/themes/%1/passwordlineedit/%2.svg")
        .arg(InternalManager::instance()->get_theme_str(), on ? "eye_on" : "eye_off");
}
