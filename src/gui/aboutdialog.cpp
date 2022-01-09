/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2021  someretical
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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->version_placeholder->setText(QApplication::applicationVersion());
    ui->version_placeholder->update();

    auto description = ui->description->text();
    ui->description->setText(description.replace("%APPNAME%", QApplication::applicationName()));
    ui->description->update();

    QFile file(":/CONTRIBUTORS.txt");
    file.open(QIODevice::ReadOnly);
    ui->contributors->setPlainText(file.readAll());

    connect(ui->ok_button, &QPushButton::clicked, this, &AboutDialog::accept, Qt::QueuedConnection);

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &AboutDialog::update_theme,
    //        Qt::QueuedConnection);
    update_theme();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::update_theme()
{
    QFile file(":/global/aboutdialog.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}
