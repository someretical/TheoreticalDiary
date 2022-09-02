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

#include <Logger.h>
#include <QAction>

#include "PasswordLineEdit.h"
#include "gui/Font.h"
#include "gui/Icons.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QLineEdit(parent)
{
    const QIcon errorIcon = icons().icon("dialog-error");
    m_actionMismatch = addAction(errorIcon, QLineEdit::TrailingPosition);
    m_actionMismatch->setVisible(false);
    m_actionMismatch->setToolTip(tr("Passwords do not match"));

    const QIcon correctIcon = icons().icon("dialog-ok");
    m_actionMatch = addAction(correctIcon, QLineEdit::TrailingPosition);
    m_actionMatch->setVisible(false);
    m_actionMatch->setToolTip(tr("Passwords match so far"));

    setEchoMode(QLineEdit::Password);
    setPlaceholderText("Password");

    QFont passwordFont = Font::fixedFont();
    passwordFont.setLetterSpacing(QFont::PercentageSpacing, 110);
    setFont(passwordFont);

    m_actionToggleVisibility = new QAction(icons().onOffIcon("password-show", false), "Toggle Password", this);
    m_actionToggleVisibility->setCheckable(true);
    addAction(m_actionToggleVisibility, QLineEdit::TrailingPosition);
    connect(m_actionToggleVisibility, &QAction::triggered, this, &PasswordLineEdit::toggleVisibility);
}

PasswordLineEdit::~PasswordLineEdit() = default;

void PasswordLineEdit::toggleVisibility(bool show)
{
    setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
    m_actionToggleVisibility->setIcon(icons().onOffIcon("password-show", show));
    m_actionToggleVisibility->setChecked(show);
}