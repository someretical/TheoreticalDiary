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

#include "DiaryTabWidget.h"
#include "DiaryWidget.h"
#include "ui_DiaryTabWidget.h"

DiaryTabWidget *DiaryTabWidget::m_instance = nullptr;

DiaryTabWidget::DiaryTabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_instance = this;

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeDiary(int)));
}

DiaryTabWidget::~DiaryTabWidget() = default;

auto DiaryTabWidget::instance() -> DiaryTabWidget *
{
    return m_instance;
}

void DiaryTabWidget::openDiary(const QString &filePath)
{
    LOG_INFO() << filePath;
}

void DiaryTabWidget::closeDiary(int widgetIndex)
{
    auto ptr = widget(widgetIndex);
    if (!ptr) {
        LOG_WARNING() << "Received tab close request from non-existent tab";
        return;
    }

    // TODO add more checks

    removeTab(widgetIndex);
}

void DiaryTabWidget::updateActions()
{
    auto ptr = qobject_cast<DiaryWidget *>(currentWidget());

    if (!ptr)
        return;

    ptr->updateActions();
}
