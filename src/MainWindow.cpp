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

#include "MainWindow.h"
#include "styling/StyleManager.h"
#include "ui_MainWindow.h"
#include "util/Util.h"
#include <Logger.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    restore_window();
    create_action_groups();
    set_icons();
}

MainWindow::~MainWindow()
{
    delete ui;
    // delete m_theme_action_group; Qt automatically deletes
}

void MainWindow::restore_window()
{
    auto settings = util::settings();
    settings.beginGroup(QStringLiteral("Window"));
    if (settings.contains(QStringLiteral("Geometry")))
        restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());

    if (settings.contains(QStringLiteral("State")))
        restoreState(settings.value(QStringLiteral("State")).toByteArray());
    settings.endGroup();
}

void MainWindow::create_action_groups()
{
    m_theme_action_group = new QActionGroup(this);
    m_theme_action_group->addAction(ui->LightTheme);
    m_theme_action_group->addAction(ui->DarkTheme);
    if (TD::Theme::Dark == StyleManager::instance()->theme())
        ui->DarkTheme->setChecked(true);
    else
        ui->LightTheme->setChecked(true);
}

void MainWindow::set_icons()
{
    auto suffix = QString(TD::Theme::Dark == StyleManager::instance()->theme() ? "_0" : "_1");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto settings = util::settings();
    settings.setValue(QStringLiteral("Window/Geometry"), saveGeometry());
    settings.setValue(QStringLiteral("Window/State"), saveState());
    LOG_INFO() << "Saved window geometry and state";

    QWidget::closeEvent(event);
}
