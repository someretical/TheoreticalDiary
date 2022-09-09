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

#include "gui/diary_menu/DiaryMainMenuWidget.h"
#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include "core/Constants.h"
#include "gui/Icons.h"
#include "ui_DiaryMainMenuWidget.h"

#include <Logger.h>

DiaryMainMenuWidget::DiaryMainMenuWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryMainMenuWidget)
{
    m_ui->setupUi(this);

    m_ui->categoryListWidget->addCategory("Calendar", icons().icon("calendar"));
    m_ui->categoryListWidget->addCategory("Statistics", icons().icon("statistics"));
    m_ui->categoryListWidget->addCategory("Pixels", icons().icon("pixels"));
    m_ui->categoryListWidget->addCategory("Search", icons().icon("system-search"));
    m_ui->categoryListWidget->addCategory("Notes", icons().icon("preferences-other"));
    m_ui->categoryListWidget->addCategory("Reminders", icons().icon("reminder"));
    m_ui->categoryListWidget->addCategory("Quotes", icons().icon("quote"));
    connect(m_ui->categoryListWidget, SIGNAL(categoryChanged(int)), m_ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
}

DiaryMainMenuWidget::~DiaryMainMenuWidget()
{
    delete m_ui;
}

DiaryWidget *DiaryMainMenuWidget::getDiaryWidget()
{
    return qobject_cast<DiaryWidget *>(parentWidget()->parentWidget()->parentWidget());
}

void DiaryMainMenuWidget::updateActions()
{
    auto widgetName = m_ui->stackedWidget->currentWidget()->objectName();

    // Diary menu
    mainWindow()->getUI()->actionSaveDiary->setEnabled(true);
    mainWindow()->getUI()->actionSaveDiaryAs->setEnabled(true);
    mainWindow()->getUI()->actionExportDiary->setEnabled(true);
    mainWindow()->getUI()->actionDiarySettings->setEnabled(true);
    mainWindow()->getUI()->actionCloseDiary->setEnabled(true);
    mainWindow()->getUI()->actionLockDiary->setEnabled(true);

    // Entry menu
    mainWindow()->getUI()->actionDeleteEntry->setEnabled(false);
    mainWindow()->getUI()->actionUpdateEntry->setEnabled(false);

    switch (TD::diaryMainMenuWidgets.value(widgetName)) {
    case TD::DiaryMainMenuWidget::Calendar:
        // Calendar menu
        mainWindow()->getUI()->actionEditSelectedEntry->setEnabled(true);
        mainWindow()->getUI()->actionDeleteSelectedEntry->setEnabled(true);
        mainWindow()->getUI()->actionJumpToToday->setEnabled(true);
        mainWindow()->getUI()->actionNextYear->setEnabled(true);
        mainWindow()->getUI()->actionPreviousYear->setEnabled(true);
        break;
    case TD::DiaryMainMenuWidget::Statistics:
        // TODO implement date navigation
    case TD::DiaryMainMenuWidget::Pixels:
        // TODO implement date navigation as well
    case TD::DiaryMainMenuWidget::Search:
    case TD::DiaryMainMenuWidget::Notes:
    case TD::DiaryMainMenuWidget::Reminders:
    case TD::DiaryMainMenuWidget::Quotes:
        // Calendar menu
        mainWindow()->getUI()->actionEditSelectedEntry->setEnabled(false);
        mainWindow()->getUI()->actionDeleteSelectedEntry->setEnabled(false);
        mainWindow()->getUI()->actionJumpToToday->setEnabled(false);
        mainWindow()->getUI()->actionNextYear->setEnabled(false);
        mainWindow()->getUI()->actionPreviousYear->setEnabled(false);
        break;
    }

    LOG_INFO() << "Updated actions for diary main menu widget" << widgetName;
}

/**
 * Selects the first item in the CategoryListWidget upon first showing the main menu
 *
 * @param event
 */
void DiaryMainMenuWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    static bool firstTime = true;
    if (firstTime) {
        m_ui->categoryListWidget->setCurrentCategory(0);
        firstTime = false;
    }
}