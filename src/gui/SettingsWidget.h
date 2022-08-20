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

#ifndef THEORETICAL_DIARY_SETTINGSWIDGET_H
#define THEORETICAL_DIARY_SETTINGSWIDGET_H

#include <QWidget>

#include "core/Constants.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsWidget;
}
QT_END_NAMESPACE

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget() override;

    void setPreviousWidget(TD::MainWindowWidget mPreviousWidget);

public slots:
    void saveSettings();
    void closeSettings();

private:
    Ui::SettingsWidget *m_ui;
    TD::MainWindowWidget m_previousWidget;

    void loadSettings();
};

#endif // THEORETICAL_DIARY_SETTINGSWIDGET_H
