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

// The code in this file was adapted from https://stackoverflow.com/a/51194796

#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QtWidgets>

#include "../core/internalmanager.h"

// The code in this file was adapted from https://stackoverflow.com/a/51194796.

class PasswordLineEdit : public QLineEdit {
public:
    PasswordLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
    {
        setEchoMode(QLineEdit::Password);

        auto monospaced = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        monospaced.setLetterSpacing(QFont::PercentageSpacing, 110);
        setFont(monospaced);

        QAction *action = addAction(QIcon(get_eye_icon(false)), QLineEdit::TrailingPosition);
        m_eye_button = qobject_cast<QToolButton *>(action->associatedWidgets().last());
        m_eye_button->setCursor(QCursor(Qt::PointingHandCursor));
        connect(m_eye_button, &QToolButton::pressed, this, &PasswordLineEdit::on_pressed, Qt::QueuedConnection);
        connect(m_eye_button, &QToolButton::released, this, &PasswordLineEdit::on_released, Qt::QueuedConnection);

        connect(InternalManager::instance(), &InternalManager::update_theme, this, &PasswordLineEdit::update_theme,
            Qt::QueuedConnection);
    }

public slots:
    void update_theme()
    {
        m_eye_button->setIcon(QIcon(get_eye_icon(true)));
    }

private slots:
    void on_pressed()
    {
        QToolButton *button = qobject_cast<QToolButton *>(sender());
        button->setIcon(QIcon(get_eye_icon(true)));
        setEchoMode(QLineEdit::Normal);
    }

    void on_released()
    {
        QToolButton *button = qobject_cast<QToolButton *>(sender());
        button->setIcon(QIcon(get_eye_icon(false)));
        setEchoMode(QLineEdit::Password);
    }

private:
    QToolButton *m_eye_button;

    QString get_eye_icon(bool const on)
    {
        return QString(":/themes/%1/%2.svg")
            .arg(InternalManager::instance()->get_theme_str(true), on ? "eye_on" : "eye_off");
    }
};

#endif // PASSWORDLINEEDIT_H
