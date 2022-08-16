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

#include <ConsoleAppender.h>
#include <FileAppender.h>
#include <Logger.h>
#include <QDir>
#include <QIcon>

#include "Application.h"
#include "core/Constants.h"
#include "core/Util.h"
#include "gui/Icons.h"
#include "gui/styling/StyleManager.h"

Application *Application::m_instance = nullptr;

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    m_instance = this;

#ifdef QT_DEBUG
    QApplication::setApplicationVersion("DEBUG");
#else
    QFile file(":/meta/version");
    file.open(QIODevice::ReadOnly);
    QApplication::setApplicationVersion(file.readAll());
#endif
    QApplication::setApplicationName("Theoretical Diary");
    QApplication::setDesktopFileName(":/meta/me.someretical.Application.desktop");
    QApplication::setWindowIcon(QIcon(QPixmap(":/icons/apps/theoreticaldiary.svg")
                                          .scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    // Remove ? button in the title bar
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton, true);

#ifdef Q_OS_WIN
    // Qt on Windows uses "MS Shell Dlg 2" as the default font for many widgets, which resolves
    // to Tahoma 8pt, whereas the correct font would be "Segoe UI" 9pt.
    // Apparently, some widgets are already using the correct font. Thanks, MuseScore for this neat fix!
    QApplication::setFont(QApplication::font("QMessageBox"));
#endif

    auto path = dataPath();
    auto log = path + "/log";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");

    QFile::remove(log);

    auto fileAppender = new FileAppender(log);
    fileAppender->setFormat(TD::FORMAT_STRING);
    auto consoleAppender = new ConsoleAppender;
    consoleAppender->setFormat(TD::FORMAT_STRING);

    cuteLogger->registerAppender(fileAppender);
    cuteLogger->registerAppender(consoleAppender);

    styleManager();
    icons();
}

Application::~Application() = default;

auto Application::instance() -> Application *
{
    return m_instance;
}
