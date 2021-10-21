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

#ifndef CONFIRMOVERWRITEBASE_H
#define CONFIRMOVERWRITEBASE_H

#include "theoreticaldiary.h"

#include <QDialog>
#include <QObject>

namespace Ui {
class ConfirmOverwriteBase;
}

class ConfirmOverwriteBase : public QDialog {
  Q_OBJECT

signals:
  void sig_complete(const td::Res code);

public:
  explicit ConfirmOverwriteBase(QWidget *parent = nullptr);
  ~ConfirmOverwriteBase();
  void action_no();
  void action_yes();

private:
  Ui::ConfirmOverwriteBase *ui;
};

template <class C> class ConfirmOverwrite : public ConfirmOverwriteBase {
public:
  /**
   * This legit took 2 days to figure out FML
   * So it turns out Q_OBJECT macro cannot be used with templates
   * That means we have to extend the base confirmoverwrite window
   * So the extended class does not need the Q_OBJECT macro
   * See https://stackoverflow.com/a/44817392 for more info
   */
  ConfirmOverwrite(void (C::*slot)(const td::Res), QWidget *parent)
      : ConfirmOverwriteBase(parent) {
    connect(this, &ConfirmOverwriteBase::sig_complete,
            qobject_cast<C *>(parent), slot);
  }
  ~ConfirmOverwrite() {}
};

#endif // CONFIRMOVERWRITEBASE_H
