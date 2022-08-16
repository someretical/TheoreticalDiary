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
#include <QDesktopServices>
#include <QUrl>

#include "Icons.h"
#include "MainMenu.h"
#include "MainWindow.h"
#include "core/Config.h"
#include "src/core/Util.h"
#include "styling/StyleManager.h"
#include "ui_MainWindow.h"

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_instance = this;
    m_ui->setupUi(this);
    m_ui->messageWidget->setHidden(true);

    setIcons();
    restoreWindow();
    createThemeActionGroup();
    connectActions();
    setupRecentlyOpenedDiaries();

    connect(m_ui->stackedWidget, SIGNAL(currentChanged(int)), SLOT(updateActions()));
    updateActions();
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    config()->set(Config::GUI_MainWindowState, saveGeometry());
    config()->set(Config::GUI_MainWindowGeometry, saveGeometry());
    LOG_INFO() << "Saved window geometry and state";

    LOG_INFO() << "Exiting application";
    QWidget::closeEvent(event);
}

void MainWindow::restoreWindow()
{
    restoreGeometry(config()->get(Config::GUI_MainWindowGeometry).toByteArray());
    restoreState(config()->get(Config::GUI_MainWindowState).toByteArray());

    LOG_INFO() << "Restored window geometry and state";
}

void MainWindow::createThemeActionGroup()
{
    m_ui->actionDarkTheme->setData(TD::Theme::Dark);
    m_ui->actionLightTheme->setData(TD::Theme::Light);
    m_themeActionGroup = new QActionGroup(this);
    m_themeActionGroup->addAction(m_ui->actionDarkTheme);
    m_themeActionGroup->addAction(m_ui->actionLightTheme);
    if (TD::Theme::Dark == config()->get(Config::GUI_Theme).toInt())
        m_ui->actionDarkTheme->setChecked(true);
    else
        m_ui->actionLightTheme->setChecked(true);

    connect(m_themeActionGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        if (config()->get(Config::GUI_Theme) == action->data())
            return;

        config()->set(Config::GUI_Theme, action->data());
        styleManager()->updateStyle();
        m_ui->statusBar->showMessage(
            QStringLiteral("Switched to %1 theme").arg(TD::Theme::Dark == action->data().toInt() ? "dark" : "light"),
            5000);
    });
}

void MainWindow::connectActions()
{
    connect(m_ui->actionReportABug, &QAction::triggered,
        []() { QDesktopServices::openUrl(QUrl("https://github.com/someretical/TheoreticalDiary/issues")); });

    connect(m_ui->actionRequestAFeature, &QAction::triggered,
        []() { QDesktopServices::openUrl(QUrl("https://github.com/someretical/TheoreticalDiary/issues")); });

    connect(m_ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void MainWindow::setIcons()
{
    // Diary menu
    m_ui->actionNewDiary->setIcon(icons()->icon("document-new"));
    m_ui->actionOpenDiary->setIcon(icons()->icon("document-open"));
    m_ui->menuOpenRecentDiary->setIcon(icons()->icon("document-open-recent"));
    m_ui->actionClearHistory->setIcon(icons()->icon("clearhistory"));
    m_ui->actionSaveDiary->setIcon(icons()->icon("document-save"));
    m_ui->actionSaveDiaryAs->setIcon(icons()->icon("document-save-as"));
    m_ui->actionImportDiary->setIcon(icons()->icon("document-import"));
    m_ui->actionExportDiary->setIcon(icons()->icon("document-export"));
    m_ui->actionDiarySettings->setIcon(icons()->icon("configure"));
    m_ui->actionLockDiary->setIcon(icons()->icon("diary-lock"));
    m_ui->actionQuit->setIcon(icons()->icon("application-exit"));

    // Calendar menu
    m_ui->actionEditSelectedEntry->setIcon(icons()->icon("calendar-edit-entry"));
    m_ui->actionDeleteSelectedEntry->setIcon(icons()->icon("calendar-delete-entry"));
    m_ui->actionJumpToToday->setIcon(icons()->icon("calendar-jump-to-today"));
    m_ui->actionNextDay->setIcon(icons()->icon("calendar-next-day"));
    m_ui->actionPreviousDay->setIcon(icons()->icon("calendar-previous-day"));
    m_ui->actionNextMonth->setIcon(icons()->icon("calendar-next-month"));
    m_ui->actionPreviousMonth->setIcon(icons()->icon("calendar-previous-month"));
    m_ui->actionNextYear->setIcon(icons()->icon("calendar-next-year"));
    m_ui->actionPreviousYear->setIcon(icons()->icon("calendar-previous-year"));

    // Entry menu
    m_ui->actionCloseEntry->setIcon(icons()->icon("entry-close"));

    // Preferences menu
    m_ui->menuTheme->setIcon(icons()->icon("palette"));
    m_ui->actionApplicationSettings->setIcon(icons()->icon("configure"));
    m_ui->actionLightTheme->setIcon(icons()->icon("theme-light"));
    m_ui->actionDarkTheme->setIcon(icons()->icon("theme-dark"));

    // Help menu
    m_ui->actionAbout->setIcon(icons()->icon("help-about"));
    m_ui->actionReportABug->setIcon(icons()->icon("bugreport"));
    m_ui->actionRequestAFeature->setIcon(icons()->icon("featurerequest"));
}

void MainWindow::updateActions()
{
    auto widgetName = m_ui->stackedWidget->currentWidget()->objectName();

    switch (TD::mainWindowWidgets.value(widgetName)) {
    case TD::MainWindowWidget::Settings:
        // Fall through here
    case TD::MainWindowWidget::MainMenu:
        break;
    case TD::MainWindowWidget::DiaryTabWidget:
        m_ui->diaryTabWidget->updateActions();
        break;
    }

    LOG_INFO() << "Updated actions for main window widget" << widgetName;
}

void MainWindow::updateActions(TD::DiaryMainMenuWidget widgetEnum)
{
    switch (widgetEnum) {
    case TD::DiaryMainMenuWidget::Calendar:
        break;
    case TD::DiaryMainMenuWidget::Statistics:
        break;
    case TD::DiaryMainMenuWidget::Pixels:
        break;
    case TD::DiaryMainMenuWidget::Search:
        break;
    case TD::DiaryMainMenuWidget::DiarySettings:
        break;
    }
}

void MainWindow::updateActions(TD::DiaryWidget widgetEnum)
{
    switch (widgetEnum) {
    case TD::DiaryWidget::UnlockPage:
        break;
    case TD::DiaryWidget::DiaryMainMenu:
        // This one is already covered by the caller
        break;
    case TD::DiaryWidget::EntryEditor:
        break;
    }
}

void MainWindow::setupRecentlyOpenedDiaries()
{
    m_clearHistoryAction = new QAction("Clear History", m_ui->menuDiary);
    m_clearHistoryAction->setIcon(icons()->icon("clearhistory"));
    m_recentDiariesActionGroup = new QActionGroup(m_ui->menuOpenRecentDiary);

    connect(m_clearHistoryAction, SIGNAL(triggered()), this, SLOT(clearRecentDiaries()));
    connect(m_recentDiariesActionGroup, &QActionGroup::triggered,
        [this](QAction *action) { openDiary(action->data().toString()); });
    connect(m_ui->menuOpenRecentDiary, SIGNAL(aboutToShow()), this, SLOT(listRecentDiaries()));
}

void MainWindow::openDiary(const QString &filePath)
{
    m_ui->diaryTabWidget->openDiary(filePath);
}

void MainWindow::clearRecentDiaries()
{
    config()->set(Config::RecentDiaries, {});
    m_ui->mainMenu->updateRecentlyOpenedDiaries();

    LOG_INFO() << "Cleared list of recently opened diaries from the menu bar";
}

void MainWindow::listRecentDiaries()
{
    m_ui->menuOpenRecentDiary->clear();

    auto recentDiaries = config()->get(Config::RecentDiaries).toStringList();
    for (const auto &recentDiary : recentDiaries) {
        QAction *action = m_ui->menuOpenRecentDiary->addAction(recentDiary);
        action->setData(recentDiary);
        m_recentDiariesActionGroup->addAction(action);
    }

    m_ui->menuOpenRecentDiary->addSeparator();
    m_ui->menuOpenRecentDiary->addAction(m_clearHistoryAction);

    LOG_INFO() << "Rendered list of recently opened diaries for menu bar";
}
