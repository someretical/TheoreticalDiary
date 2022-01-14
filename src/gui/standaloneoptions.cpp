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

#include "standaloneoptions.h"
#include "optionsmenu.h"
#include "ui_standaloneoptions.h"

StandaloneOptions::StandaloneOptions(QWidget *parent) : QWidget(parent), ui(new Ui::StandaloneOptions)
{
    ui->setupUi(this);
    ui->settings_frame->layout()->addWidget(new OptionsMenu(false, this));

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &StandaloneOptions::update_theme,
    //        Qt::QueuedConnection);
    update_theme();
}

StandaloneOptions::~StandaloneOptions()
{
    delete ui;
}

void StandaloneOptions::update_theme()
{
    QFile file(":/global/standaloneoptions.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}
