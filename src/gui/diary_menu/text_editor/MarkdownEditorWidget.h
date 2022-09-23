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

#ifndef THEORETICAL_DIARY_MARKDOWNEDITORWIDGET_H
#define THEORETICAL_DIARY_MARKDOWNEDITORWIDGET_H

#include "gui/diary_menu/text_editor/qplaintexteditsearchwidget.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MarkdownEditorWidget;
}
QT_END_NAMESPACE

class MarkdownEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = nullptr);
    ~MarkdownEditorWidget() override;

    uint getDebounceDelay() const;

private:
    Ui::MarkdownEditorWidget *m_ui;
    QPlainTextEditSearchWidget *m_searchWidget;
    uint m_deboundeDelay = 0;
};

#endif // THEORETICAL_DIARY_MARKDOWNEDITORWIDGET_H
