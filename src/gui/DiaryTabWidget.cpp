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
#include <QFileInfo>

#include "DiaryTabWidget.h"
#include "MainWindow.h"
#include "core/Config.h"
#include "gui/editor/DiaryWidget.h"
#include "ui_DiaryTabWidget.h"
#include "ui_MainWindow.h"

DiaryTabWidget *DiaryTabWidget::m_instance = nullptr;

DiaryTabWidget::DiaryTabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_instance = this;

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(updateWindowTitle(int)));
    connect(
        this, &DiaryTabWidget::tabCloseRequested, this, [this](int index) { closeDiaryTab(index, false); },
        Qt::QueuedConnection);
}

DiaryTabWidget::~DiaryTabWidget() = default;

auto DiaryTabWidget::instance() -> DiaryTabWidget *
{
    return m_instance;
}

void DiaryTabWidget::openDiary(const QString &filePath)
{
    auto exists = QFile::exists(filePath);
    auto recentDiaries = config()->get(Config::RecentDiaries).toStringList();

    if (!exists) {
        mainWindow()->getUI()->messageWidget->showMessage(
            "<b>Error:</b> This file no longer exists", MessageWidget::Error);
        recentDiaries.removeOne(filePath);
        LOG_INFO() << "Removed invalid file path from recently opened diary list" << filePath;
    }
    else {
        recentDiaries.prepend(filePath);
        recentDiaries.removeDuplicates();
        auto maxLength = config()->get(Config::MaximumRecentDiaries).toInt();
        while (recentDiaries.size() > maxLength) {
            recentDiaries.removeLast();
        }
        LOG_INFO() << "Added file path to recently opened diary list" << filePath;

        auto skip = false;
        for (int i = 0; i < count(); ++i) {
            if (filePath == qobject_cast<DiaryWidget *>(widget(i))->getFilePath()) {
                skip = true;
                setCurrentIndex(i);

                LOG_INFO() << "Opened existing diary tab and switched to index" << i << filePath;
            }
        }

        if (!skip) {
            mainWindow()->changeCurrentWidget(TD::MainWindowWidget::DiaryTabWidget);
            LOG_INFO() << "Opening new diary tab for" << filePath;

            auto tab = new DiaryWidget(filePath, this);
            QFileInfo info(filePath);
            auto index = addTab(tab, QStringLiteral("%1 [Locked]").arg(info.fileName()));
            setCurrentIndex(index);
        }
    }

    config()->set(Config::RecentDiaries, recentDiaries);
}

void DiaryTabWidget::closeDiaryTab(int widgetIndex, bool skipChecks)
{
    auto ptr = widget(widgetIndex);
    if (!ptr) {
        LOG_WARNING() << "Received tab close request from non-existent tab";
        return;
    }

    if (!skipChecks) {
        // TODO add more checks
    }

    LOG_INFO() << "Removing tab for diary" << qobject_cast<DiaryWidget *>(ptr)->getFilePath();
    removeTab(widgetIndex);

    if (!count()) {
        LOG_INFO() << "Switching back to main menu because no tabs left";
        mainWindow()->changeCurrentWidget(TD::MainWindowWidget::MainMenu);
    }
}

void DiaryTabWidget::updateActions()
{
    auto ptr = qobject_cast<DiaryWidget *>(currentWidget());

    if (!ptr)
        // It is expected that this will return when the widget goes from 0 tabs to 1 tab
        return;

    ptr->updateActions();
}

void DiaryTabWidget::updateWindowTitle(int index)
{
    mainWindow()->setWindowTitle(QStringLiteral("%1 - %2").arg(tabText(index), QApplication::applicationName()));
}
