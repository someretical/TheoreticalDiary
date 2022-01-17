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
#include "../util/diarypixellabel.h"
#include "../util/misc.h"
#include "ui_diarypixels.h"

char const *MONTH_LETTERS = "JFMAMJJASOND";
int const LABEL_SIZE = 36;

DiaryPixels::DiaryPixels(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryPixels)
{
    ui->setupUi(this);
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

void DiaryPixels::update_theme() {}

int DiaryPixels::calculate_size()
{
    // Minimum dimensions:
    // Width of button 36px
    // Width of frame 1218px
    // Height of frame 570px
    // 32 labels * 36px per row
    // 31 gaps of 2px

    updateGeometry();

    // For every 500px width/height, increase gap by 1px.
    int gap = 0;
    int w = ui->hidden_frame->width();
    int h = ui->hidden_frame->height();
    int top = std::max({w, h, 1218});
    while ((top -= 500) > -1)
        gap++;

    ui->grid->setSpacing(gap);

    // This approach is not entirely ideal but it's the most read friendly since there are no calculations of ratios and
    // all that stuff.
    int ideal = LABEL_SIZE;
    while (
        // ideal * 31 because max 31 days in a row.
        // gap * 31 because there are 32 labels max in a row.
        // 36 is the width of the single letter label marking the month.
        ((ideal * 31) + (gap * 31) + 36) < w &&
        // ideal * 12 because there are 12 rows.
        // gap * 11 since there are 11 gaps in between 12 rows.
        ((ideal * 12) + (gap * 11)) < h)
        ++ideal;

    return ideal;
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

        ui->grid->addWidget(label, 0, 0, 1, 1, Qt::AlignCenter);

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

                auto ptr = new DiaryPixelLabel(td::Rating::Unknown, false, tmp_date, size, this);
                connect(this, &DiaryPixels::sig_changed_size, ptr, &DiaryPixelLabel::resize, Qt::QueuedConnection);

                ui->grid->addWidget(
                    ptr, month, day + 1 /* +1 here because of the month label added at the start of each row */);
            }

            continue;
        }

        // This code runs if some/all dates in a month exist.
        for (int day = 0; day < days; ++day) {
            auto const &entrymap = iter->second;
            auto const &iter2 = entrymap.find(day + 1 /* day is index 1 based */);

            tmp_date.setDate(year, month + 1, day + 1);
            if (iter2 == entrymap.end()) {
                auto ptr = new DiaryPixelLabel(td::Rating::Unknown, false, tmp_date, size, this);
                connect(this, &DiaryPixels::sig_changed_size, ptr, &DiaryPixelLabel::resize, Qt::QueuedConnection);

                ui->grid->addWidget(ptr, month, day + 1);
            }
            else {
                auto const &[important, rating, dummy, d2] = iter2->second;

                auto ptr = new DiaryPixelLabel(rating, important, tmp_date, size, this);
                connect(this, &DiaryPixels::sig_changed_size, ptr, &DiaryPixelLabel::resize, Qt::QueuedConnection);

                ui->grid->addWidget(ptr, month, day + 1);
            }
        }
    }

    InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    qDebug() << "Rendered pixels grid" << current_date;
}

void DiaryPixels::export_image()
{
    auto year = QString::number(ui->year_edit->date().year());
    auto const &filename = QFileDialog::getSaveFileName(
        this, "Export image", QString("%1/%2.png").arg(QDir::homePath(), year), "Images (*.png);;All files");

    if (filename.isEmpty())
        return;

    QPixmap pixmap;

    if (ui->add_year_checkbox->isChecked()) {
        auto frame = ui->hidden_frame->grab();
        pixmap = QPixmap(frame.size().width(), frame.size().height() + 50);
        pixmap.fill(frame.toImage().pixel(0, 0));

        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawPixmap(0, 50, frame);

        auto bold_font = QApplication::font();
        bold_font.setBold(true);
        bold_font.setPointSize(20);
        p.setFont(bold_font);

        QRectF rect;
        rect = p.boundingRect(pixmap.rect(), Qt::AlignTop | Qt::AlignHCenter, year);
        rect.translate(0, 10);

        p.setPen(misc::theme_to_text(InternalManager::instance()->get_theme(true)));
        p.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, year);
    }
    else {
        pixmap = ui->hidden_frame->grab();
    }

    if (pixmap.save(filename))
        cmb::ok_messagebox(
            this, []() {}, "Image exported.", "");
    else
        cmb::ok_messagebox(
            this, []() {}, "The app could not write to the specified location.",
            "This usually means a permission error.");
}

void DiaryPixels::resizeEvent(QResizeEvent *)
{
    emit sig_changed_size(calculate_size());
}
