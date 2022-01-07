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

#include "licensesdialog.h"
#include "ui_licensesdialog.h"

LicensesDialog::LicensesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LicensesDialog)
{
    ui->setupUi(this);

    QFile file(":/LICENSES.txt");
    file.open(QIODevice::ReadOnly);
    ui->licenses->setPlainText(file.readAll());

    connect(ui->ok_button, &QPushButton::clicked, this, &LicensesDialog::accept, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &LicensesDialog::update_theme,
        Qt::QueuedConnection);
    update_theme();
}

LicensesDialog::~LicensesDialog()
{
    delete ui;
}

void LicensesDialog::update_theme()
{
    QFile file(":/global/licensesdialog.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}
