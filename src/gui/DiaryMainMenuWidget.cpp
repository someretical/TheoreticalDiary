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

#include "DiaryMainMenuWidget.h"
#include "MainWindow.h"
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
    mainWindow()->updateActions(TD::diaryMainMenuWidgets.value(widgetName));
    LOG_INFO() << "Updated actions for diary main menu widget" << widgetName;
}
