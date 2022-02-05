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
    setup_grid();
    ui->year_edit->setDate(QDate::currentDate());

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryPixels::render_grid,
        Qt::QueuedConnection);
    connect(ui->year_edit, &QDateEdit::dateChanged, this, &DiaryPixels::render_grid, Qt::QueuedConnection);
    connect(ui->export_img_button, &QPushButton::clicked, this, &DiaryPixels::export_image, Qt::QueuedConnection);
    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryPixels::~DiaryPixels()
{
    delete ui;
}

int DiaryPixels::calculate_size()
{
    // Minimum dimensions:
    // Width of frame 1218px
    // Height of frame 570px

    updateGeometry();

    int gap = 0;
    int w = ui->hidden_frame->width() - 18; // - 18 because the hidden_frame has margins of 9px on all sides.
    int h = ui->hidden_frame->height() - 18;
    int top = std::max({w, h, 1218});

    // For every 500px width/height, increase gap by 1px.
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

void DiaryPixels::setup_grid()
{
    auto const size = calculate_size();

    for (int month = 1; month < 13; ++month) {
        auto label = new QLabel(QString(MONTH_LETTERS[month - 1]), this);
        QFont f = label->font();
        f.setPointSize(14);
        f.setBold(true);
        label->setFont(f);
        label->setFixedHeight(LABEL_SIZE);
        label->setFixedWidth(LABEL_SIZE);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->grid->addWidget(label, month - 1, 0);

        for (int day = 1; day < 32; ++day) {
            auto ptr = new DiaryPixelLabel(td::Rating::Unknown, false, month, day, size, this);
            connect(this, &DiaryPixels::sig_changed_size, ptr, &DiaryPixelLabel::resize_slot, Qt::QueuedConnection);

            ui->grid->addWidget(ptr, month - 1, day);
        }
    }

    qDebug() << "Rendered pixel grid.";
}

void DiaryPixels::render_grid(QDate const &new_date)
{
    current_date = new_date;
    ui->year_edit->blockSignals(true);
    ui->year_edit->setDate(new_date);
    ui->year_edit->blockSignals(false);
    auto const &opt = DiaryHolder::instance()->get_yearmap(current_date);

    for (int month = 1; month < 13; ++month) {
        int days = QDate(current_date.year(), month, 1).daysInMonth();

        auto placeholder = [this, month, days]() {
            for (int day = 1; day < 32; ++day) {
                auto pixel = qobject_cast<DiaryPixelLabel *>(ui->grid->itemAtPosition(month - 1, day)->widget());

                if (day > days)
                    pixel->setVisible(false);
                else
                    pixel->setVisible(true);

                pixel->rating = td::Rating::Unknown;
                pixel->special = false;
                pixel->update();
            }
        };

        if (opt) {
            auto const &monthmap = (*opt)->second;
            auto const &iter = monthmap.find(month);

            if (iter == monthmap.end()) {
                placeholder();
            }
            else {
                auto const &entrymap = iter->second;

                for (int day = 1; day < 32; ++day) {
                    auto pixel = qobject_cast<DiaryPixelLabel *>(ui->grid->itemAtPosition(month - 1, day)->widget());
                    auto const &iter2 = entrymap.find(day);

                    if (day > days)
                        pixel->setVisible(false);
                    else
                        pixel->setVisible(true);

                    if (iter2 == entrymap.end()) {
                        pixel->rating = td::Rating::Unknown;
                        pixel->special = false;
                    }
                    else {
                        auto const &[important, rating, dummy, d2] = iter2->second;
                        pixel->rating = rating;
                        pixel->special = important;
                    }

                    pixel->update();
                }
            }
        }
        else {
            placeholder();
        }
    }

    qDebug() << "Updated pixels grid" << current_date;
}

void DiaryPixels::export_image()
{
    auto year = QString::number(ui->year_edit->date().year());
    auto const &filename = QFileDialog::getSaveFileName(
        this, "Export image", QString("%1/%2.png").arg(QDir::homePath(), year), "Images (*.png);;All files");

    if (filename.isEmpty())
        return;

    AppBusyLock lock;
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
        bold_font.setPointSize(24);
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

    if (pixmap.save(filename)) {
        auto cb = [this, filename](int const res) {
            if (QMessageBox::Yes != res)
                return;

            QFileInfo info(filename);
            if (!QDesktopServices::openUrl(info.dir().path())) {
                auto msgbox = new QMessageBox(this);
                msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
                msgbox->setText("Failed to show file in explorer.");
                msgbox->setStandardButtons(QMessageBox::Ok);
                return msgbox->show();
            }
        };

        auto msgbox = new QMessageBox(this);
        msgbox->setAttribute(Qt::WA_DeleteOnClose, true);
        msgbox->setText("Would you like to open the folder containing the exported image?");
        msgbox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgbox->setDefaultButton(QMessageBox::No);
        connect(msgbox, &QMessageBox::finished, cb);
        msgbox->show();
    }
    else {
        return cmb::display_local_io_error(this);
    }
}

void DiaryPixels::resizeEvent(QResizeEvent *)
{
    emit sig_changed_size(calculate_size());
}
