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

#ifndef THOERETICALDIARY_H
#define THOERETICALDIARY_H

// Forward declaration
class GoogleWrapper;

#include "../util/encryptor.h"
#include "diaryholder.h"
#include "googlewrapper.h"

#include <QtWidgets>
#include <json.hpp>
#include <sys/stat.h>

// This is required so std::string can be passed via signals and slots.
Q_DECLARE_METATYPE(std::string)

namespace td {
enum Res : int { Yes, No };
} // namespace td

/*
 * TheoreticalDiary class
 */
class TheoreticalDiary : public QApplication {
  Q_OBJECT

signals:
  void sig_begin_hash(const std::string &plaintext);
  void apply_theme();

public:
  TheoreticalDiary(int &argc, char **argv);
  ~TheoreticalDiary();
  static TheoreticalDiary *instance();

  QString data_location();
  QString theme();
  bool confirm_overwrite(QWidget *p);

  GoogleWrapper *gwrapper;
  DiaryHolder *diary_holder;
  Encryptor *encryptor;
  QSettings *settings;

  // Change trackers
  bool diary_modified;
  bool diary_file_modified;

  // During asynchronous operations like password hashing and network requests,
  // the window should not be able to be closed.
  bool closeable;

  // See https://doc.qt.io/qt-5/qthread.html for multithreading
  QThread worker_thread;
  QString *application_theme;

  // Cached stylesheets
  QString *danger_button_style;

public slots:
  void diary_changed();
  void diary_file_changed();

private:
  void load_fonts();
};

// The set_key() function is blocking
// This means that if it is called from the same thread as the GUI processes
// events, the GUI will freeze during hashing.
// Obviously, this is undesirable behaviour so the function should be performed
// in a worker on a separate thread.

class HashWorker : public QObject {
  Q_OBJECT

public slots:
  void hash(const std::string &plaintext) {
    TheoreticalDiary::instance()->encryptor->set_key(plaintext);
    emit done();
  }

signals:
  void done(const bool do_decrypt = true);
};

#endif // THOERETICALDIARY_H
