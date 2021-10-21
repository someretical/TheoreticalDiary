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

#ifndef PROMPTPASSWORD_H
#define PROMPTPASSWORD_H

#include "theoreticaldiary.h"

#include <QDialog>
#include <string>

namespace Ui {
class PromptPassword;
}

class PromptPassword : public QDialog {
  Q_OBJECT

signals:
  void sig_complete(const td::Res code);

public:
  explicit PromptPassword(const std::string &uncompressed,
                          QWidget *parent = nullptr);
  ~PromptPassword();

public slots:
  void decrypt();
  void close_window();
  void toggle_pwd();

private:
  Ui::PromptPassword *ui;
  std::string *to_be_decrypted;
};

#endif // PROMPTPASSWORD_H
