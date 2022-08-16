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
#include <QFile>
#include <QFileDialog>

#include "MainMenu.h"
#include "MainWindow.h"
#include "ui_MainMenu.h"

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), m_ui(new Ui::MainMenu)
{
    m_ui->setupUi(this);
    m_ui->versionText->setText(QStringLiteral("Version %1").arg(QApplication::applicationVersion()));
    QPixmap icon(":/icons/apps/theoreticaldiary.svg");
    m_ui->labelIcon->setPixmap(icon.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    connect(m_ui->buttonOpenExistingDiary, SIGNAL(clicked(bool)), this, SLOT(openExistingDiary()));
}

MainMenu::~MainMenu()
{
    delete m_ui;
}

void MainMenu::updateRecentlyOpenedDiaries() {}

void MainMenu::openExistingDiary()
{
    auto filePath = QFileDialog::getOpenFileName(this, "Open Diary", QDir::homePath(), "Diary (*.dat);;All files");
    mainWindow()->openDiary(filePath);
}
