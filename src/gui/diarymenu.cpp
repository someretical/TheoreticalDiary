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

#include "diarymenu.h"
#include "../core/theoreticaldiary.h"
#include "diaryeditor.h"
#include "diaryentryviewer.h"
#include "diarypixels.h"
#include "diarystats.h"
#include "mainwindow.h"
#include "ui_diarymenu.h"

#include <json.hpp>

DiaryMenu::DiaryMenu(QDate const &date, QWidget *parent) : QWidget(parent), ui(new Ui::DiaryMenu)
{
    ui->setupUi(this);

    first_created = date;

    // When changes are made in the editor, the other tabs need to know about it
    // so they can update accordingly.
    auto const diary_editor = new DiaryEditor(this);
    ui->editor->layout()->addWidget(diary_editor);
    ui->entries->layout()->addWidget(new DiaryEntryViewer(diary_editor, this));
    ui->statistics->layout()->addWidget(new DiaryStats(diary_editor, this));
    ui->pixels->layout()->addWidget(new DiaryPixels(diary_editor, this));

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &DiaryMenu::tab_changed, Qt::QueuedConnection);

    connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this, &DiaryMenu::apply_theme,
        Qt::QueuedConnection);
    apply_theme();
}

DiaryMenu::~DiaryMenu()
{
    delete ui;
}

void DiaryMenu::apply_theme()
{
    QFile file(QString(":/%1/diarymenu.qss").arg(TheoreticalDiary::instance()->theme()));
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();
}

void DiaryMenu::tab_changed(int const tab)
{
    switch (tab) {
    // Editor tab
    case 0:
        break;
    // List tab
    case 1:
        break;
    // Stats tab
    case 2:
        break;
    // Pixels tab
    case 3:
        break;
    // Options tab
    case 4:
        qobject_cast<MainWindow *>(parentWidget()->parentWidget())->show_options_menu();
        break;
    }
}

QString DiaryMenu::get_day_suffix(int const day)
{
    switch (day) {
    case 1:
    case 21:
    case 31:
        return "st";
    case 2:
    case 22:
        return "nd";
    case 3:
    case 23:
        return "rd";
    default:
        return "th";
    }
}
