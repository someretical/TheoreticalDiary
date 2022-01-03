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

#include "custommessageboxes.h"

int const MAX_LINE_LENGTH = 100;

void extend_top_line(std::string &top)
{
    if (top.size() < MAX_LINE_LENGTH)
        top.append(MAX_LINE_LENGTH - top.size(), ' ');
}

namespace td {
int ok_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom)
{
    QMessageBox msgbox(parent);
    QPushButton ok_button("OK", &msgbox);
    ok_button.setFlat(true);
    QFont f = ok_button.font();
    f.setPointSize(11);
    ok_button.setFont(f);

    msgbox.setFont(f);
    extend_top_line(top);
    msgbox.setText(QString::fromStdString(top));
    msgbox.setInformativeText(QString::fromStdString(bottom));
    msgbox.addButton(&ok_button, QMessageBox::AcceptRole);
    msgbox.setDefaultButton(&ok_button);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}

int yn_messagebox(QWidget *parent, std::string &&top, std::string const &&bottom)
{
    QMessageBox msgbox(parent);

    QPushButton yes("YES", &msgbox);
    QFont f = yes.font();
    f.setPointSize(11);
    yes.setFont(f);
    yes.setStyleSheet(TheoreticalDiary::instance()->danger_button_style);
    QPushButton no("NO", &msgbox);
    no.setFlat(true);
    no.setFont(f);

    msgbox.setFont(f);
    extend_top_line(top);
    msgbox.setText(QString::fromStdString(top));
    msgbox.setInformativeText(QString::fromStdString(bottom));
    msgbox.addButton(&yes, QMessageBox::AcceptRole);
    msgbox.addButton(&no, QMessageBox::RejectRole);
    msgbox.setDefaultButton(&no);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}

int ync_messagebox(QWidget *parent, std::string const &&accept_text, std::string const &&reject_text,
    std::string const &&destroy_text, std::string &&top, std::string const &&bottom)
{
    QMessageBox msgbox(parent);

    QPushButton destroy_button(QString::fromStdString(destroy_text), &msgbox);
    QFont f = destroy_button.font();
    f.setPointSize(11);
    destroy_button.setFont(f);
    destroy_button.setStyleSheet(TheoreticalDiary::instance()->danger_button_style);

    QPushButton accept_button(QString::fromStdString(accept_text), &msgbox);
    accept_button.setFont(f);
    QPushButton cancel_button(QString::fromStdString(reject_text), &msgbox);
    cancel_button.setFlat(true);
    cancel_button.setFont(f);
    msgbox.setFont(f);
    extend_top_line(top);
    msgbox.setText(QString::fromStdString(top));
    msgbox.setInformativeText(QString::fromStdString(bottom));
    msgbox.addButton(&accept_button, QMessageBox::AcceptRole);
    msgbox.addButton(&cancel_button, QMessageBox::RejectRole);
    msgbox.addButton(&destroy_button, QMessageBox::DestructiveRole);
    msgbox.setDefaultButton(&accept_button);
    msgbox.setTextInteractionFlags(Qt::NoTextInteraction);

    return msgbox.exec();
}
} // namespace td
