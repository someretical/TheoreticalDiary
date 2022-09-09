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

#ifndef THEORETICAL_DIARY_MAINWINDOW_H
#define THEORETICAL_DIARY_MAINWINDOW_H

#include "gui/DiaryTabWidget.h"
#include "gui/MessageWidget.h"
#include "core/Constants.h"

#include <QActionGroup>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static auto instance() -> MainWindow *;

    void openDiary(const QString &path);
    void changeCurrentWidget(TD::MainWindowWidget widget);
    auto getUI() const -> Ui::MainWindow *;

private:
    void restoreWindow();
    void createThemeActionGroup();
    void connectActions();
    void setIcons();
    void setupRecentlyOpenedDiaries();

    static MainWindow *m_instance;
    Ui::MainWindow *m_ui;

private:
    QAction *m_clearHistoryAction;
    QActionGroup *m_themeActionGroup;
    QActionGroup *m_recentDiariesActionGroup;

private slots:
    void updateActions();
    void clearRecentDiaries();
    void listRecentDiaries();

protected:
    void closeEvent(QCloseEvent *event) override;
};

inline auto mainWindow() -> MainWindow *
{
    return MainWindow::instance();
}
#endif // THEORETICAL_DIARY_MAINWINDOW_H
