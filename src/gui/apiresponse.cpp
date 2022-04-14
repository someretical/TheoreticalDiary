/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
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

APIResponse::APIResponse(QByteArray const &res, QWidget *parent) : QDialog(parent), m_ui(new Ui::APIResponse)
{
    m_ui->setupUi(this);
    m_ui->res->setPlainText(res);

    auto monospaced = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monospaced.setLetterSpacing(QFont::PercentageSpacing, 110);
    m_ui->res->setFont(monospaced);

    connect(m_ui->ok_button, &QPushButton::clicked, this, &APIResponse::accept, Qt::QueuedConnection);
}

APIResponse::~APIResponse()
{
    delete m_ui;
}
