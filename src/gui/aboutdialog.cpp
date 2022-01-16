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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

// clang-format off
QString const DESCRIPTION = R"(
<p>%APPNAME% is a desktop GUI application for keeping a digital diary.</p>
<p>Please provide the info below when reporting a bug.</p>
)";

QString const ABOUT = R"(
<p><b>Version</b> %VERSION%<p>
<p><b>Revision</b> %REVISION%<p>
<p><b>OS</b> %OS%<p>
<p><b>CPU</b> %CPU%<p>
<p><b>Kernel</b> %KERNEL%<p>
)";

// Shhhhhh ;)
QString const CONTRIBUTORS = R"(
<ul>
    <li><a href="https://github.com/someretical">someretical</a> - Creator</li>
    <li><a href="https://github.com/quezec">quezec</a> - Bug reporter and feature suggester</li>
    <li><a href="https://www.youtube.com/watch?v=dQw4w9WgXcQ">You</a> - The user</li>
</ul>
)";

QString const LIBRARIES = R"(
<ul>
    <li><a href="https://github.com/someretical/TheoreticalDiary">Theoretical Diary</a> (<a href="https://github.com/someretical/TheoreticalDiary/blob/master/LICENSE">license</a>)</li>
    <li><a href="https://github.com/benlau/asyncfuture">AsyncFuture</a> (<a href="https://github.com/benlau/asyncfuture/blob/master/LICENSE">license</a>)</li>
    <li><a href="https://www.hsluv.org/">HSLuv-C: Human-friendly HSL</a> (<a href="https://github.com/hsluv/hsluv/blob/master/LICENSE">license</a>)</li>
    <li><a href="https://json.nlohmann.me/">JSON for Modern C++</a> (<a href="https://github.com/nlohmann/json/blob/develop/LICENSE.MIT">license</a>)</li>
    <li><a href="https://keepassxc.org/">KeepassXC</a> (<a href="https://github.com/keepassxreboot/keepassxc/blob/develop/LICENSE.GPL-3">license</a>)</li>
    <li><a href="https://google.github.io/material-design-icons/">Material Design icons</a> (<a href="https://github.com/google/material-design-icons/blob/master/LICENSE">license</a>)</li>
    <li><a href="https://www.qt.io/">Qt</a> (<a href="https://www.gnu.org/licenses/gpl-3.0.en.html">license</a>)</li>
    <li><a href="https://www.cryptopp.com/">cryptopp</a> (<a href="https://github.com/weidai11/cryptopp/blob/master/License.txt">license</a>)</li>
</ul>
)";
// clang-format on

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->description->setText(QString(DESCRIPTION).replace("%APPNAME%", QApplication::applicationName()));
    ui->contributors->setText(CONTRIBUTORS);
    ui->licenses->setText(LIBRARIES);

    QFile file(":/REVISION.txt");
    file.open(QIODevice::ReadOnly);

    ui->about->setText(QString(ABOUT)
                           .replace("%VERSION%", QApplication::applicationVersion())
                           .replace("%REVISION%", file.readAll())
                           .replace("%OS%", QSysInfo::prettyProductName())
                           .replace("%CPU%", QSysInfo::currentCpuArchitecture())
                           .replace("%KERNEL%", QSysInfo::kernelType() + " " + QSysInfo::kernelVersion()));

    connect(ui->ok_button, &QPushButton::clicked, this, &AboutDialog::accept, Qt::QueuedConnection);
    connect(ui->clipboard, &QPushButton::clicked,
        [this]() { QGuiApplication::clipboard()->setText(ui->about->toPlainText()); });

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &AboutDialog::update_theme,
    //        Qt::QueuedConnection);
    //    update_theme();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::update_theme() {}
