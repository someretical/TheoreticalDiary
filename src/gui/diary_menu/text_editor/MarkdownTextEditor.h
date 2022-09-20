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

#ifndef THEORETICAL_DIARY_MARKDOWNTEXTEDITOR_H
#define THEORETICAL_DIARY_MARKDOWNTEXTEDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MarkdownTextEditor;
}
QT_END_NAMESPACE

class MarkdownTextEditor : public QWidget {
    Q_OBJECT

public:
    explicit MarkdownTextEditor(QWidget *parent = nullptr);
    ~MarkdownTextEditor() override;

private:
    Ui::MarkdownTextEditor *m_ui;

private slots:
    void toggleAdvancedSearch(bool checked);
};

#endif // THEORETICAL_DIARY_MARKDOWNTEXTEDITOR_H
