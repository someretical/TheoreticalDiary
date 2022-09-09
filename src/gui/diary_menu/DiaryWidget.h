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

#ifndef THEORETICAL_DIARY_DIARYWIDGET_H
#define THEORETICAL_DIARY_DIARYWIDGET_H

#include "core/Constants.h"
#include "core/Diary.h"
#include "crypto/DiaryKey.h"
#include "gui/DiaryTabWidget.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DiaryWidget;
}
QT_END_NAMESPACE

class DiaryWidget : public QWidget {
    Q_OBJECT

public:
    explicit DiaryWidget(const QString &filePath, QWidget *parent = nullptr);
    ~DiaryWidget() override;

    auto getDiary() -> Diary &;
    auto getFilePath() const -> const QString &;
    void changeCurrentWidget(TD::DiaryWidget widget);

public slots:
    void updateActions();

private:
    Ui::DiaryWidget *m_ui;
    QString m_filePath;
    DiaryKey m_diaryKey;
    Diary m_diary;
};

#endif // THEORETICAL_DIARY_DIARYWIDGET_H
