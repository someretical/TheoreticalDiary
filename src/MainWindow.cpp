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

#include "MainWindow.h"
#include "core/Icons.h"
#include "styling/StyleManager.h"
#include "ui_MainWindow.h"
#include "util/Util.h"

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_instance = this;
    m_ui->setupUi(this);

    restoreWindow();
    createActionGroups();
    setIcons();
}

MainWindow::~MainWindow()
{
    delete m_ui;
    // delete m_ThemeActionGroup; Qt automatically deletes
}

auto MainWindow::instance() -> MainWindow *
{
    return m_instance;
}

void MainWindow::restoreWindow()
{
    auto settings = util::settings();
    settings.beginGroup("Window");
    if (settings.contains("Geometry"))
        restoreGeometry(settings.value("Geometry").toByteArray());

    if (settings.contains("State"))
        restoreState(settings.value("State").toByteArray());
    settings.endGroup();
}

void MainWindow::createActionGroups()
{
    m_ThemeActionGroup = new QActionGroup(this);
    m_ThemeActionGroup->addAction(m_ui->LightTheme);
    m_ThemeActionGroup->addAction(m_ui->DarkTheme);
    if (TD::Theme::Dark == styleManager()->theme())
        m_ui->DarkTheme->setChecked(true);
    else
        m_ui->LightTheme->setChecked(true);
}

void MainWindow::setIcons()
{
    // Diary menu
    m_ui->NewDiary->setIcon(icons()->icon("document-new"));
    m_ui->OpenDiary->setIcon(icons()->icon("document-open"));
    m_ui->MenuOpenRecentDiary->setIcon(icons()->icon("document-open-recent"));
    m_ui->ClearHistory->setIcon(icons()->icon("clearhistory"));
    m_ui->SaveDiary->setIcon(icons()->icon("document-save"));
    m_ui->SaveDiaryAs->setIcon(icons()->icon("document-save-as"));
    m_ui->ImportDiary->setIcon(icons()->icon("document-import"));
    m_ui->ExportDiary->setIcon(icons()->icon("document-export"));
    m_ui->LockDiary->setIcon(icons()->icon("diary-lock"));
    m_ui->UploadDiary->setIcon(icons()->icon("diary-upload"));
    m_ui->DownloadDiary->setIcon(icons()->icon("diary-download"));
    m_ui->ConnectGoogleDrive->setIcon(icons()->icon("connect-google-drive"));
    m_ui->ExitApplication->setIcon(icons()->icon("application-exit"));

    // Entry menu
    m_ui->EditEntry->setIcon(icons()->icon("entry-edit"));
    m_ui->DeleteEntry->setIcon(icons()->icon("entry-delete"));
    m_ui->JumpToToday->setIcon(icons()->icon("entry-jump"));
    m_ui->SearchEntries->setIcon(icons()->icon("system-search"));

    // Preferences menu
    m_ui->MenuTheme->setIcon(icons()->icon("palette"));
    m_ui->Settings->setIcon(icons()->icon("configure"));

    // Help menu
    m_ui->About->setIcon(icons()->icon("help-about"));
    m_ui->ReportABug->setIcon(icons()->icon("bugreport"));
    m_ui->RequestAFeature->setIcon(icons()->icon("featurerequest"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto settings = util::settings();
    settings.setValue("Window/Geometry", saveGeometry());
    settings.setValue("Window/State", saveState());
    LOG_INFO() << "Saved window geometry and state";

    LOG_INFO() << "Exiting application";
    QWidget::closeEvent(event);
}