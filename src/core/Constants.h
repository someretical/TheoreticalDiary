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

#ifndef THEORETICAL_DIARY_CONSTANTS_H
#define THEORETICAL_DIARY_CONSTANTS_H

#include <QHash>
#define QS QStringLiteral

namespace TD {
static const char *FORMAT_STRING = "[%{Type}] <%{Function}:%{line}> %{message}\n";
enum Theme { Dark, Light };
enum ColourRole { Text, Unknown, VeryBad, Bad, Ok, Good, VeryGood };

enum class MainWindowWidget { MainMenu, DiaryTabWidget, Settings };
static const QHash<QString, MainWindowWidget> mainWindowWidgets = {{QS("pageMainMenu"), MainWindowWidget::MainMenu},
    {QS("pageDiaryTabWidget"), MainWindowWidget::DiaryTabWidget}, {QS("pageSettings"), MainWindowWidget::Settings}};

enum class DiaryWidget { UnlockPage, DiaryMainMenu, EntryEditor };
static const QHash<QString, DiaryWidget> diaryWidgets = {{QS("pageUnlock"), DiaryWidget::UnlockPage},
    {QS("pageDiaryMainMenu"), DiaryWidget::DiaryMainMenu}, {QS("pageEntryEditor"), DiaryWidget::EntryEditor}};

enum class DiaryMainMenuWidget { Calendar, Statistics, Pixels, Search, DiarySettings };
static const QHash<QString, DiaryMainMenuWidget> diaryMainMenuWidgets = {
    {QS("pageCalendar"), DiaryMainMenuWidget::Calendar},
    {QS("pageStatistics"), DiaryMainMenuWidget::Statistics},
    {QS("pagePixels"), DiaryMainMenuWidget::Pixels},
    {QS("pageSearch"), DiaryMainMenuWidget::Search},
    {QS("pageDiaryMenu"), DiaryMainMenuWidget::DiarySettings},
};
} // namespace TD

#endif // THEORETICAL_DIARY_CONSTANTS_H
