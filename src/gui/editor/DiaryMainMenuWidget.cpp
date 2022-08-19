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

#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include "DiaryMainMenuWidget.h"
#include "core/Constants.h"
#include "ui_DiaryMainMenuWidget.h"

DiaryMainMenuWidget::DiaryMainMenuWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryMainMenuWidget)
{
    m_ui->setupUi(this);
}

DiaryMainMenuWidget::~DiaryMainMenuWidget()
{
    delete m_ui;
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
        mainWindow()->getUI()->actionNextDay->setEnabled(true);
        mainWindow()->getUI()->actionPreviousDay->setEnabled(true);
        mainWindow()->getUI()->actionNextMonth->setEnabled(true);
        mainWindow()->getUI()->actionPreviousMonth->setEnabled(true);
        mainWindow()->getUI()->actionNextYear->setEnabled(true);
        mainWindow()->getUI()->actionPreviousYear->setEnabled(true);
        break;
    case TD::DiaryMainMenuWidget::Statistics:
        // Fall through
    case TD::DiaryMainMenuWidget::Pixels:
        // Fall through
    case TD::DiaryMainMenuWidget::Search:
        // Fall through
    case TD::DiaryMainMenuWidget::DiarySettings:
        // Calendar menu
        mainWindow()->getUI()->actionEditSelectedEntry->setEnabled(false);
        mainWindow()->getUI()->actionDeleteSelectedEntry->setEnabled(false);
        mainWindow()->getUI()->actionJumpToToday->setEnabled(false);
        mainWindow()->getUI()->actionNextDay->setEnabled(false);
        mainWindow()->getUI()->actionPreviousDay->setEnabled(false);
        mainWindow()->getUI()->actionNextMonth->setEnabled(false);
        mainWindow()->getUI()->actionPreviousMonth->setEnabled(false);
        mainWindow()->getUI()->actionNextYear->setEnabled(false);
        mainWindow()->getUI()->actionPreviousYear->setEnabled(false);
        break;
    }

    LOG_INFO() << "Updated actions for diary main menu widget" << widgetName;
}
