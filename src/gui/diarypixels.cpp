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

#include "diarypixels.h"
#include "../core/diaryholder.h"
#include "../util/custommessageboxes.h"
#include "../util/misc.h"
#include "ui_diarypixels.h"

char const *MONTH_LETTERS = "JFMAMJJASOND";
int const LABEL_SIZE = 36;

DiaryPixels::DiaryPixels(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryPixels)
{
    ui->setupUi(this);

    rating_stylesheets = std::vector<std::unique_ptr<QString>>();
    black_star = QString("");
    white_star = QString("");

    ui->year_edit->setDate(QDate::currentDate());

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryPixels::render_grid,
        Qt::QueuedConnection);
    connect(ui->render_button, &QPushButton::clicked, this, &DiaryPixels::render_button_clicked, Qt::QueuedConnection);
    connect(ui->export_img_button, &QPushButton::clicked, this, &DiaryPixels::export_image, Qt::QueuedConnection);

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryPixels::update_theme,
    //        Qt::QueuedConnection);
    update_theme();

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryPixels::~DiaryPixels()
{
    delete ui;
}

void DiaryPixels::update_theme()
{
    QFile file(":/global/diarypixels.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();

    file.setFileName(":/global/small_white_star.qss");
    file.open(QIODevice::ReadOnly);
    white_star = file.readAll();
    file.close();

    file.setFileName(":/global/small_black_star.qss");
    file.open(QIODevice::ReadOnly);
    black_star = file.readAll();
    file.close();

    //    for (auto &ss_ptr : rating_stylesheets)
    //        ss_ptr.reset();

    //    rating_stylesheets.clear();

    for (int i = 0; i < 6; ++i) {
        file.setFileName(QString(":/global/pixels/%1.qss").arg(QString::number(i)));
        file.open(QIODevice::ReadOnly);
        rating_stylesheets.push_back(std::make_unique<QString>(file.readAll()));
        file.close();
    }
}

int DiaryPixels::calculate_size()
{
    // Minimum dimensions:
    // Width of button 36px
    // Width of frame 1218px
    // Height of frame 570px
    // 32 labels * 36px per row
    // 31 gaps of 2px

    // For every 500px width/height, increase gap by 1px.
    int gap = 0;
    int top = std::max({ui->hidden_frame->width(), ui->hidden_frame->height(), 1218});
    while ((top -= 500) > -1)
        gap++;

    ui->grid->setSpacing(gap);

    int final_width =
        ((ui->hidden_frame->width() - (gap * 31) /* 31 gaps */ - 36) /* month letter dimensions stay the same */) /
        31; /* max number of days in a month */

    int final_height = (ui->hidden_frame->height() - (gap * 11)) / 36;

    return std::max({LABEL_SIZE, final_width, final_height});
}

void DiaryPixels::render_button_clicked()
{
    render_grid(ui->year_edit->date());
}

void DiaryPixels::render_grid(QDate const &new_date)
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);

    // Remove everything from current grid.
    QLayoutItem *child;
    while ((child = ui->grid->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    current_date = new_date;
    auto const &opt = DiaryHolder::instance()->get_yearmap(current_date);

    // Set new grid.
    if (!opt) {
        auto label = new QLabel("It seems there are no entries yet for this year...", this);
        auto f = label->font();
        f.setPointSize(11);
        label->setFont(f);

        ui->grid->addWidget(label, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

        return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    }

    auto &&tmp_date = QDate::currentDate();
    auto const year = current_date.year();
    auto const size = calculate_size();

    for (int month = 0; month < 12; ++month) {
        auto label = new QLabel(QString(MONTH_LETTERS[month]), this);
        QFont f = label->font();
        f.setPointSize(14);
        f.setBold(true);
        label->setFont(f);
        label->setFixedHeight(LABEL_SIZE);
        label->setFixedWidth(LABEL_SIZE);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->grid->addWidget(label, month, 0);

        int days = QDate(current_date.year(), month + 1, 1).daysInMonth();

        // This block runs if the month doesn't exist at all.
        auto const &monthmap = (*opt)->second;
        auto const &iter = monthmap.find(month + 1 /* Month is index 1 based */);
        if (iter == monthmap.end()) {
            for (int day = 0; day < days; ++day) {
                tmp_date.setDate(year, month + 1, day + 1);
                ui->grid->addWidget(new PixelLabel(td::Rating::Unknown, false, tmp_date, size, this), month,
                    day + 1 /* +1 here because of the month label added at the start of each row */);
            }

            continue;
        }

        // This code runs if some/all dates in a month exist.
        for (int day = 0; day < days; ++day) {
            auto const &entrymap = iter->second;
            auto const &iter2 = entrymap.find(day + 1 /* day is index 1 based */);

            tmp_date.setDate(year, month + 1, day + 1);
            if (iter2 == entrymap.end()) {
                ui->grid->addWidget(new PixelLabel(td::Rating::Unknown, false, tmp_date, size, this), month, day + 1);
            }
            else {
                auto const &[important, rating, dummy, d2] = iter2->second;
                ui->grid->addWidget(new PixelLabel(rating, important, tmp_date, size, this), month, day + 1);
            }
        }
    }

    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    qDebug() << "Rendered pixels grid" << current_date;
}

void DiaryPixels::export_image()
{
    auto const &filename = QFileDialog::getSaveFileName(this, "Export image",
        QString("%1/%2.png").arg(QDir::homePath(), QString::number(ui->year_edit->date().year())),
        "Images (*.png);;All files");

    if (filename.isEmpty())
        return;

    // Thanks to https://stackoverflow.com/a/24341699
    if (ui->hidden_frame->grab().save(filename))
        td::ok_messagebox(this, "Image exported.", "");
    else
        td::ok_messagebox(this, "Export failed.", "The app could not write to the specified location.");
}

void DiaryPixels::resizeEvent(QResizeEvent *)
{
    emit sig_changed_size(calculate_size());
}

/*
 * PixelLabel class
 */
PixelLabel::PixelLabel(td::Rating const r, bool const special, QDate const &date, int const size, QWidget *parent)
    : QLabel(parent)
{
    auto p = qobject_cast<DiaryPixels *>(parent);

    setFixedHeight(size);
    setFixedWidth(size);

    auto stylesheet = *p->rating_stylesheets[static_cast<int>(r)];

    connect(p, &DiaryPixels::sig_changed_size, this, &PixelLabel::resize, Qt::QueuedConnection);

    // Set background star if necessary.
    if (special) {
        switch (r) {
        case td::Rating::Unknown:
            // Fall through
        case td::Rating::VeryBad:
            // Fall through
        case td::Rating::Bad:
            // Fall through
        case td::Rating::Ok:
            stylesheet.append(qobject_cast<DiaryPixels *>(parent)->white_star);
            break;
        case td::Rating::Good:
            // Fall through
        case td::Rating::VeryGood:
            stylesheet.append(qobject_cast<DiaryPixels *>(parent)->black_star);
            break;
        }
    }

    setStyleSheet(stylesheet);

    setToolTip(
        QString("%1 %2%3").arg(date.toString("MMMM"), QString::number(date.day()), misc::get_day_suffix(date.day())));
}

PixelLabel::~PixelLabel() {}

void PixelLabel::resize(int const new_width)
{
    setFixedHeight(new_width);
    setFixedWidth(new_width);
}
