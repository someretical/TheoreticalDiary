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
    <li><a href="https://github.com/benlau/asyncfuture">AsyncFuture</a> (<a href="https://github.com/benlau/asyncfuture/blob/master/LICENSE">Apache License 2.0</a>)</li>
    <li><a href="https://www.cryptopp.com/">cryptopp</a> (<a href="https://github.com/weidai11/cryptopp/blob/master/License.txt">Boost Software License 1.0</a>)</li>
    <li><a href="https://www.hsluv.org/">HSLuv-C: Human-friendly HSL</a> (<a href="https://github.com/hsluv/hsluv/blob/master/LICENSE">MIT License</a>)</li>
    <li><a href="https://json.nlohmann.me/">JSON for Modern C++</a> (<a href="https://github.com/nlohmann/json/blob/develop/LICENSE.MIT">MIT License</a>)</li>
    <li><a href="https://keepassxc.org/">KeepassXC</a> (<a href="https://github.com/keepassxreboot/keepassxc/blob/develop/LICENSE.GPL-3">GNU General Public License v3.0</a>)</li>
    <li><a href="https://google.github.io/material-design-icons/">Material Design icons</a> (<a href="https://github.com/google/material-design-icons/blob/master/LICENSE">Apache License 2.0</a>)</li>
    <li><a href="https://github.com/pipacs/o2">o2</a> (<a href="https://github.com/pipacs/o2/blob/master/LICENSE">BSD 2-Clause "Simplified" License</a>)</li>
    <li><a href="https://www.qt.io/">Qt</a> (<a href="https://www.gnu.org/licenses/gpl-3.0.en.html">GNU General Public License v3.0</a>)</li>
    <li><a href="https://github.com/someretical/TheoreticalDiary">Theoretical Diary</a> (<a href="https://github.com/someretical/TheoreticalDiary/blob/master/LICENSE">GNU General Public License v3.0</a>)</li>
</ul>
)";
// clang-format on

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
    m_ui->description->setText(QString(DESCRIPTION).replace("%APPNAME%", QApplication::applicationName()));
    m_ui->contributors->setText(CONTRIBUTORS);
    m_ui->licenses->setText(LIBRARIES);

    QFile file(":/REVISION.txt");
    file.open(QIODevice::ReadOnly);

    m_ui->about->setText(QString(ABOUT)
                             .replace("%VERSION%", QApplication::applicationVersion())
                             .replace("%REVISION%", file.readAll())
                             .replace("%OS%", QSysInfo::prettyProductName())
                             .replace("%CPU%", QSysInfo::currentCpuArchitecture())
                             .replace("%KERNEL%", QSysInfo::kernelType() + " " + QSysInfo::kernelVersion()));

    connect(m_ui->ok_button, &QPushButton::clicked, this, &AboutDialog::accept, Qt::QueuedConnection);
    connect(m_ui->clipboard, &QPushButton::clicked,
        [this]() { QGuiApplication::clipboard()->setText(m_ui->about->toPlainText()); });
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}
