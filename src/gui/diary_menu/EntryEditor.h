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

#ifndef THEORETICAL_DIARY_ENTRYEDITOR_H
#define THEORETICAL_DIARY_ENTRYEDITOR_H

#include "gui/diary_menu/DiaryWidget.h"

#include <QWidget>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui {
class EntryEditor;
}
QT_END_NAMESPACE

class EntryEditor : public QWidget {
    Q_OBJECT

public:
    explicit EntryEditor(QWidget *parent = nullptr);
    ~EntryEditor() override;

    DiaryWidget *getDiaryWidget();
    void changeDate(const QDate &date);

public slots:
    void saveEntry();
    void closeEntry();

private:
    Ui::EntryEditor *m_ui;
    QDate m_currentDate;
};

#endif // THEORETICAL_DIARY_ENTRYEDITOR_H
