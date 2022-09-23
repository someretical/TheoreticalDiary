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

#include "MarkdownEditorWidget.h"
#include "ui_MarkdownEditorWidget.h"

#include <Logger.h>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::MarkdownEditorWidget)
{
    m_ui->setupUi(this);
    m_searchWidget = new QPlainTextEditSearchWidget;
    m_searchWidget->setDebounceDelay(m_deboundeDelay);
    m_searchWidget->setTextEdit(m_ui->plainTextEdit);
    m_ui->verticalLayout->addWidget(m_searchWidget);
    m_ui->plainTextEdit->setSearchWidget(m_searchWidget);
    m_ui->plainTextEdit->setLineNumberEnabled(true);
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete m_ui;
    // m_searchWidget is deleted by the vertical layout
}

uint MarkdownEditorWidget::getDebounceDelay() const
{
    return m_deboundeDelay;
}
