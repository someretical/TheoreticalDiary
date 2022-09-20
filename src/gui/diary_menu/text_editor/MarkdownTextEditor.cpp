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

#include "MarkdownTextEditor.h"
#include "ui_MarkdownTextEditor.h"
#include "gui/Icons.h"

#include <Logger.h>

MarkdownTextEditor::MarkdownTextEditor(QWidget *parent) : QWidget(parent), m_ui(new Ui::MarkdownTextEditor)
{
    m_ui->setupUi(this);

    m_ui->searchNext->setIcon(icons().icon("go-next"));
    m_ui->searchPrevious->setIcon(icons().icon("go-previous"));
    m_ui->searchFirst->setIcon(icons().icon("go-first"));
    m_ui->searchLast->setIcon(icons().icon("go-last"));
    m_ui->searchClose->setIcon(icons().icon("window-close"));
    m_ui->replaceToggleButton->setIcon(icons().icon("menu-expanded"));

    connect(m_ui->replaceToggleButton, SIGNAL(toggled(bool)), this, SLOT(toggleAdvancedSearch(bool)));
    connect(m_ui->searchClose, SIGNAL(clicked(bool)), m_ui->searchFrame, SLOT(hide()));
    m_ui->replaceToggleButton->toggle();
}

MarkdownTextEditor::~MarkdownTextEditor()
{
    delete m_ui;
}

void MarkdownTextEditor::toggleAdvancedSearch(bool checked = false)
{
    if (checked) {
        m_ui->replaceToggleButton->setIcon(icons().icon("menu-expanded"));
    } else {
        m_ui->replaceToggleButton->setIcon(icons().icon("menu-hidden"));
    }

    m_ui->replaceLabel->setVisible(checked);
    m_ui->replaceLineEdit->setVisible(checked);
    m_ui->searchFirst->setVisible(checked);
    m_ui->searchLast->setVisible(checked);
    m_ui->modeLabel->setVisible(checked);
    m_ui->modeComboBox->setVisible(checked);
    m_ui->replaceButton->setVisible(checked);
    m_ui->replaceAllButton->setVisible(checked);
}
