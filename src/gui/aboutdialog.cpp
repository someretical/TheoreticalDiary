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
#include "../core/theoreticaldiary.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->version_placeholder->setText(QApplication::applicationVersion());

    QFile file(":/CONTRIBUTORS.txt");
    file.open(QIODevice::ReadOnly);
    ui->contributors->setPlainText(file.readAll());
    file.close();

    connect(ui->ok_button, &QPushButton::clicked, this, &AboutDialog::accept, Qt::QueuedConnection);

    connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this, &AboutDialog::apply_theme,
        Qt::QueuedConnection);
    apply_theme();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::apply_theme()
{
    QFile file(":/global/aboutdialog.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();
}
