/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "diarymenu.h"
#include "diaryeditor.h"
#include "diaryentryviewer.h"
#include "diarypixels.h"
#include "diarystats.h"
#include "optionsmenu.h"
#include "ui_diarymenu.h"

DiaryMenu::DiaryMenu(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryMenu)
{
    ui->setupUi(this);

    auto date = QDate::currentDate();

    ui->entries->layout()->addWidget(new DiaryEntryViewer(this));
    ui->statistics->layout()->addWidget(new DiaryStats(this));
    ui->pixels->layout()->addWidget(new DiaryPixels(this));
    // DiaryEditor has to be initialised last because it tells all the other widgets to update.
    ui->editor->layout()->addWidget(new DiaryEditor(date, this));
    ui->settings_tab->layout()->addWidget(new OptionsMenu(true, this));

    connect(ui->diary_menu_tab, &QTabWidget::currentChanged, this, &DiaryMenu::tab_changed, Qt::QueuedConnection);

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryMenu::update_theme,
    //        Qt::QueuedConnection);
    //    update_theme();
}

DiaryMenu::~DiaryMenu()
{
    delete ui;
}

void DiaryMenu::update_theme() {}

void DiaryMenu::tab_changed(int const)
{
    qDebug() << "Switched to tab:" << ui->diary_menu_tab->tabText(ui->diary_menu_tab->currentIndex());
}
