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

#include "promptauth.h"
#include "googlewrapper.h"
#include "theoreticaldiary.h"
#include "ui_promptauth.h"

#include <QAction>

PromptAuth::PromptAuth(QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptAuth) {
  ui->setupUi(this);

  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, this, &PromptAuth::auto_close);

  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptAuth::reject,
          Qt::QueuedConnection);
}

PromptAuth::~PromptAuth() { delete ui; }

void PromptAuth::reject() {
  TheoreticalDiary::instance()->gwrapper->auth_err();
  /**
   * The window does not need to be closed here since
   * auth_err() actually emits sig_oauth2_complete
   * which is connected to auto_close() below
   * meaning, this window is closed anyway
   */
}

void PromptAuth::auto_close() { accept(); }
