/**
 * This file is part of Theoretical Diary.
 *
 * Theoretical Diary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Theoretical Diary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef THOERETICALDIARY_H
#define THOERETICALDIARY_H

class GoogleWrapper;

#include "diaryholder.h"
#include "encryptor.h"
#include "googlewrapper.h"

#include <QApplication>
#include <QObject>
#include <QThread>
#include <json.hpp>
#include <string>

// This is required so std::string can be passed via signals and slots.
Q_DECLARE_METATYPE(std::string)

namespace td {
struct LocalSettings {
  std::string bak1_id;
  std::string bak2_id;
  bool sync_enabled;
};

inline void to_json(nlohmann::json &j, const LocalSettings &s) {
  j = nlohmann::json{{"bak1_id", s.bak1_id},
                     {"bak2_id", s.bak2_id},
                     {"sync_enabled", s.sync_enabled}};
}

inline void from_json(const nlohmann::json &j, LocalSettings &s) {
  j.at("bak1_id").get_to<std::string>(s.bak1_id);
  j.at("bak2_id").get_to<std::string>(s.bak2_id);
  j.at("sync_enabled").get_to<bool>(s.sync_enabled);
}

enum Res : int { Yes, No };
} // namespace td

class TheoreticalDiary : public QApplication {
  Q_OBJECT

signals:
  void sig_begin_hash(const std::string &plaintext);

public:
  TheoreticalDiary(int &argc, char **argv);
  ~TheoreticalDiary();
  static TheoreticalDiary *instance();

  void load_settings();
  bool save_settings();

  GoogleWrapper *gwrapper;
  DiaryHolder *diary_holder;
  Encryptor *encryptor;
  td::LocalSettings *local_settings;
  bool *unsaved_changes;

  // See https://doc.qt.io/qt-5/qthread.html for multithreading
  QThread worker_thread;

public slots:
  void changes_made();
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
  void done();
};

#endif // THOERETICALDIARY_H
