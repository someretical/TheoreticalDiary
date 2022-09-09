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

#include "gui/diary_menu/DiaryWidget.h"
#include "../ui_MainWindow.h"
#include "core/Constants.h"
#include "gui/MainWindow.h"
#include "ui_DiaryWidget.h"

#include <Logger.h>

DiaryWidget::DiaryWidget(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_ui(new Ui::DiaryWidget), m_filePath(filePath), m_diaryKey()
{
    m_ui->setupUi(this);
    m_ui->diaryUnlockWidget->setFilePath(filePath);

    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
}

DiaryWidget::~DiaryWidget()
{
    delete m_ui;
}

auto DiaryWidget::getDiary() -> Diary &
{
    return m_diary;
}

void DiaryWidget::updateActions()
{
    auto currentWidget = m_ui->stackedWidget->currentWidget();
    auto widgetEnum = TD::diaryWidgets.value(currentWidget->objectName());

    switch (widgetEnum) {
    case TD::DiaryWidget::UnlockPage:
        // Diary menu
        mainWindow()->getUI()->actionSaveDiary->setEnabled(false);
        mainWindow()->getUI()->actionSaveDiaryAs->setEnabled(false);
        mainWindow()->getUI()->actionExportDiary->setEnabled(false);
        mainWindow()->getUI()->actionDiarySettings->setEnabled(false);
        mainWindow()->getUI()->actionCloseDiary->setEnabled(true);
        mainWindow()->getUI()->actionLockDiary->setEnabled(false);

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

        // Entry menu
        mainWindow()->getUI()->actionDeleteEntry->setEnabled(false);
        mainWindow()->getUI()->actionUpdateEntry->setEnabled(false);
        break;
    case TD::DiaryWidget::DiaryMainMenu:
        m_ui->diaryMainMenuWidget->updateActions();
        break;
    case TD::DiaryWidget::EntryEditor:
        // Diary menu
        mainWindow()->getUI()->actionSaveDiary->setEnabled(true);
        mainWindow()->getUI()->actionSaveDiaryAs->setEnabled(true);
        mainWindow()->getUI()->actionExportDiary->setEnabled(true);
        mainWindow()->getUI()->actionDiarySettings->setEnabled(true);
        mainWindow()->getUI()->actionCloseDiary->setEnabled(true);
        mainWindow()->getUI()->actionLockDiary->setEnabled(true);

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

        // Entry menu
        mainWindow()->getUI()->actionDeleteEntry->setEnabled(true);
        mainWindow()->getUI()->actionUpdateEntry->setEnabled(true);
        break;
    }

    LOG_INFO() << "Updated actions for diary widget" << currentWidget->objectName();
}

auto DiaryWidget::getFilePath() const -> const QString &
{
    return m_filePath;
}

void DiaryWidget::changeCurrentWidget(TD::DiaryWidget widget)
{
    switch (widget) {
    case TD::DiaryWidget::UnlockPage:
        m_ui->stackedWidget->setCurrentIndex(0);
        break;
    case TD::DiaryWidget::DiaryMainMenu:
        m_ui->stackedWidget->setCurrentIndex(1);
        break;
    case TD::DiaryWidget::EntryEditor:
        m_ui->stackedWidget->setCurrentIndex(2);
        break;
    }
}
