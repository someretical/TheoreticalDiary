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

#include "gui/SettingsWidget.h"
#include "core/Config.h"
#include "core/Constants.h"
#include "gui/Icons.h"
#include "gui/MainWindow.h"
#include "ui_SettingsWidget.h"

#include <Logger.h>
#include <QStatusBar>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);
    m_ui->categoryListWidget->addCategory("General", icons().icon("preferences-other"));
    m_ui->categoryListWidget->addCategory("Security", icons().icon("preferences-security"));
    connect(m_ui->categoryListWidget, SIGNAL(categoryChanged(int)), m_ui->stackedWidget, SLOT(setCurrentIndex(int)));

    m_ui->pieChartSortButtonGroup->setId(m_ui->pieChartSortByRatingRadioButton, Config::PieChart_Rating);
    m_ui->pieChartSortButtonGroup->setId(m_ui->pieChartSortByNumberOfDaysRadioButton, Config::PieChart_Days);
    connect(m_ui->pieChartSortButtonGroup, &QButtonGroup::idClicked, [this](int index) {
        m_ui->pieChartRatingComboBox->setEnabled(Config::PieChart_Rating == index);
        m_ui->pieChartNumberOfDaysComboBox->setEnabled(Config::PieChart_Days == index);

        auto pieChartSortType = config()->get(Config::PieChartSortOrder).toInt();
        if (index == Config::PieChart_Rating) {
            m_ui->pieChartRatingComboBox->setCurrentIndex(pieChartSortType);
        }
        else {
            m_ui->pieChartNumberOfDaysComboBox->setCurrentIndex(pieChartSortType);
        }
    });

    connect(
        m_ui->diaryLockTimeoutCheckBox, SIGNAL(toggled(bool)), m_ui->diaryLockTimeoutSpinBox, SLOT(setEnabled(bool)));

    m_previousWidget = TD::MainWindowWidget::MainMenu;

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(closeSettings()));

    loadSettings();
}

SettingsWidget::~SettingsWidget()
{
    delete m_ui;
}

/**
 * Stores the enum ID of the previously open widget before the settings widget was switched to
 *
 * @param mPreviousWidget
 */
void SettingsWidget::setPreviousWidget(TD::MainWindowWidget mPreviousWidget)
{
    m_previousWidget = mPreviousWidget;
}

void SettingsWidget::saveSettings()
{
    config()->set(Config::Diary_SaveOnLock, m_ui->autoSaveWhenLockCheckBox->isChecked());
    config()->set(Config::Diary_BackupBeforeSave, m_ui->backupBeforeSaveCheckBox->isChecked());

    auto pieChartSort = m_ui->pieChartSortButtonGroup->checkedId();
    config()->set(Config::PieChartSortType, pieChartSort);
    config()->set(Config::PieChartSortOrder, Config::PieChart_Rating == pieChartSort
                                                 ? m_ui->pieChartRatingComboBox->currentIndex()
                                                 : m_ui->pieChartNumberOfDaysComboBox->currentIndex());

    config()->set(Config::Diary_LockTimeoutEnabled, m_ui->diaryLockTimeoutCheckBox->isChecked());
    config()->set(Config::Diary_LockTimeout, m_ui->diaryLockTimeoutSpinBox->value());

    mainWindow()->statusBar()->showMessage("Saved settings");

    closeSettings();
}

void SettingsWidget::closeSettings()
{
    LOG_INFO() << "Exiting settings widget";

    mainWindow()->changeCurrentWidget(m_previousWidget);
}

void SettingsWidget::loadSettings()
{
    m_ui->autoSaveWhenLockCheckBox->setChecked(config()->get(Config::Diary_SaveOnLock).toBool());
    m_ui->backupBeforeSaveCheckBox->setChecked(config()->get(Config::Diary_BackupBeforeSave).toBool());

    auto pieChartSortType = config()->get(Config::PieChartSortType).toInt();
    if (Config::PieChart_Days == pieChartSortType) {
        m_ui->pieChartSortByNumberOfDaysRadioButton->setChecked(true);
    }
    else {
        m_ui->pieChartSortByRatingRadioButton->setChecked(true);
    }
    emit m_ui->pieChartSortButtonGroup->idClicked(pieChartSortType);

    auto lockTimeoutEnabled = config()->get(Config::Diary_LockTimeoutEnabled).toBool();
    m_ui->diaryLockTimeoutCheckBox->setChecked(lockTimeoutEnabled);
    m_ui->diaryLockTimeoutSpinBox->setEnabled(lockTimeoutEnabled);
    m_ui->diaryLockTimeoutSpinBox->setValue(config()->get(Config::Diary_LockTimeout).toInt());
}
