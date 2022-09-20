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

#include "EntryEditor.h"
#include "ui_EntryEditor.h"
#include "gui/Icons.h"

#include <QAbstractButton>
#include <QTextEdit>

EntryEditor::EntryEditor(QWidget *parent) : QWidget(parent), m_ui(new Ui::EntryEditor)
{
    m_ui->setupUi(this);

    auto font = QApplication::font();
    font.setPointSize(font.pointSize() + 2);
    font.setBold(true);
    m_ui->dateLabel->setFont(font);

    m_ui->categoryListWidget->addCategory("Edit TODO", icons().icon("configure"));
    m_ui->categoryListWidget->addCategory("Properties", icons().icon("configure"));

    connect(m_ui->categoryListWidget, SIGNAL(categoryChanged(int)), m_ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveEntry()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(closeEntry()));

    m_ui->plainTextEdit->setLineNumberEnabled(true);
}

EntryEditor::~EntryEditor()
{
    delete m_ui;
}

DiaryWidget *EntryEditor::getDiaryWidget()
{
    return qobject_cast<DiaryWidget *>(parentWidget()->parentWidget()->parentWidget());
}

/**
 * Updates the widget with the data associated with the provided date from the diary data structure.
 *
 * @param date Year, month and day all matter
 */
void EntryEditor::changeDate(const QDate &date) {

}

void EntryEditor::saveEntry() {}

void EntryEditor::closeEntry() {}
