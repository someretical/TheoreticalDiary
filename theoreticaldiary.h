/**
 * This file is part of theoretical-diary.
 *
 * theoretical-diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * theoretical-diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THOERETICALDIARY_H
#define THOERETICALDIARY_H

#include "diaryholder.h"
#include "googlewrapper.h"
#include "settingsprovider.h"

#include <QApplication>
#include <QObject>

class TheoreticalDiary : public QApplication {
  Q_OBJECT

public:
  TheoreticalDiary(int &argc, char **argv);
  ~TheoreticalDiary();
  static TheoreticalDiary *instance();
  GoogleWrapper *gwrapper;
  DiaryHolder *diary_holder;
  SettingsProvider *settings_provider;
  bool *unsaved_changes;

public slots:
  void changes_made();
};

#endif // THOERETICALDIARY_H
