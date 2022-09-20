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

#include "gui/MainMenu.h"
#include "core/Config.h"
#include "gui/Icons.h"
#include "gui/MainWindow.h"
#include "ui_MainMenu.h"
#include "ui_MainWindow.h"

#include <Logger.h>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), m_ui(new Ui::MainMenu)
{
    m_ui->setupUi(this);
    m_ui->versionText->setText(QStringLiteral("Version %1").arg(QApplication::applicationVersion()));
    m_ui->labelIcon->setPixmap(QPixmap(":/icons/application/scalable/apps/theoreticaldiary.svg")
                                   .scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    connect(m_ui->buttonOpenExistingDiary, SIGNAL(clicked(bool)), this, SLOT(openExistingDiary()));
    connect(m_ui->recentDiariesList, &QListWidget::itemActivated,
        [](QListWidgetItem *item) { mainWindow()->openDiary(item->text()); });
    connect(m_ui->buttonImportDiary, &QPushButton::clicked, []() {
        mainWindow()->getUI()->messageWidget->showMessage(
            "This is a test messageThis is a test messageThis is a test messageThis is a test messageThis is a test "
            "messageThis is a test messageThis is a test messageThis is a test message",
            KMessageWidget::Information);
        // TODO implement
    });
    connect(m_ui->buttonSettings, &QPushButton::clicked, this,
        []() { mainWindow()->getUI()->actionApplicationSettings->trigger(); });
    connect(
        m_ui->buttonQuit, &QPushButton::clicked, this, []() { mainWindow()->close(); }, Qt::QueuedConnection);
}

MainMenu::~MainMenu()
{
    delete m_ui;
}

/**
 * Generates the QListWidget which displays the list of recently opened diaries. This function also sets the welcome
 * text depending on if there are any recently opened diaries.
 */
void MainMenu::updateRecentlyOpenedDiaries()
{
    m_ui->recentDiariesList->clear();

    auto recentDiaryList = config()->get(Config::RecentDiaries).toStringList();
    for (auto &diaryFile : recentDiaryList) {
        auto item = new QListWidgetItem{};
        item->setText(diaryFile);
        m_ui->recentDiariesList->addItem(item);
    }

    if (m_ui->recentDiariesList->count() > 0) {
        m_ui->recentDiariesWidget->setVisible(true);
        m_ui->welcomeLabel->setText("Welcome Back to");
        m_ui->buttonCreateNewDiary->setDefault(false);
    }
    else {
        m_ui->recentDiariesWidget->setVisible(false);
        m_ui->welcomeLabel->setText("Welcome to");
        m_ui->buttonCreateNewDiary->setDefault(true);
    }
    LOG_INFO() << "Updated recently opened diary list in the main menu";
}

void MainMenu::openExistingDiary()
{
    auto filePath = QFileDialog::getOpenFileName(this, "Open Diary", QDir::homePath(), "Diary (*.td);;All files (*)");
    if (filePath.size())
        mainWindow()->openDiary(filePath);
}

/**
 * Captures key press events for:
 * - Navigating the recently opened diares QListWidget
 * - Deleting recently opened diaries from the QListWidget
 *
 * @param event
 */
void MainMenu::keyPressEvent(QKeyEvent *event)
{
    if (m_ui->recentDiariesList->hasFocus()) {
        auto currentItem = m_ui->recentDiariesList->currentItem();
        if (!currentItem || currentItem->text().isEmpty()) {
            return QWidget::keyPressEvent(event);
        }

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            return mainWindow()->openDiary(currentItem->text());
        }
        else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
            auto recentDiaries = config()->get(Config::RecentDiaries).toStringList();
            recentDiaries.removeOne(currentItem->text());
            config()->set(Config::RecentDiaries, recentDiaries);

            mainWindow()->statusBar()->showMessage(QStringLiteral("Removed file %1").arg(currentItem->text()));
            LOG_INFO() << "Removed file path from recently opened diary list" << currentItem->text();
            return updateRecentlyOpenedDiaries();
        }
    }

    QWidget::keyPressEvent(event);
}

/**
 * Additionally calls updateRecentlyOpenedDiaries
 *
 * @param event
 */
void MainMenu::showEvent(QShowEvent *event)
{
    updateRecentlyOpenedDiaries();
    QWidget::showEvent(event);
}
