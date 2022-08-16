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
#include "DiaryWidget.h"
#include "MainWindow.h"
#include "core/Constants.h"
#include "ui_DiaryWidget.h"

DiaryWidget::DiaryWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
}

DiaryWidget::~DiaryWidget()
{
    delete m_ui;
}

void DiaryWidget::updateActions()
{
    auto currentWidget = m_ui->stackedWidget->currentWidget();
    auto widgetEnum = TD::diaryWidgets.value(currentWidget->objectName());

    if (TD::DiaryWidget::DiaryMainMenu == widgetEnum)
        qobject_cast<DiaryMainMenuWidget *>(currentWidget)->updateActions();
    else
        mainWindow()->updateActions(widgetEnum);

    LOG_INFO() << "Updated actions for diary widget" << currentWidget->objectName();
}
