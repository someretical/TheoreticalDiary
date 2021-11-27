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

#include "apiresponse.h"
#include "ui_apiresponse.h"

APIResponse::APIResponse(QByteArray &res, QWidget *parent)
    : QDialog(parent), ui(new Ui::APIResponse) {
  ui->setupUi(this);
  ui->res->setPlainText(res);

  connect(ui->ok_button, &QPushButton::clicked, this, &APIResponse::accept,
          Qt::QueuedConnection);

  setStyleSheet("QPlainTextEdit { font-family: \"Roboto Mono\" }");
}

APIResponse::~APIResponse() { delete ui; }