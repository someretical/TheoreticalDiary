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

#include "diaryentryviewer.h"
#include "../core/diaryholder.h"
#include "../util/misc.h"
#include "ui_diaryentryviewer.h"

int const MAX_LINE_LEN = 110;
int const DAY_LABEL_SIZE = 50;
int const SIZE = 50;

const char *PLACEHOLDER_TEXT = R"(
<center><p>It seems there are no entries yet for this month...</p></center>
)";

char const *TABLE_START = "<table border=0 cellspacing=15>";

char const *SINGLE_ROW = R"(
<tr>
<td valign=top><img src="data:image/png;base64, %1" alt="" width="50" height="50"/></td>
<td valign=middle>%2</td>
</tr>
)";

const QString HR_ROW(R"(
<tr>
<td></td>
<td><hr></td>
</tr>
)");

char const *TABLE_END = "</table>";

DiaryEntryViewer::DiaryEntryViewer(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryEntryViewer)
{
    ui->setupUi(this);

    rating_stylesheets = std::vector<std::unique_ptr<QString>>();
    black_star = QString();
    white_star = QString();

    // Navigator slots
    connect(ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &DiaryEntryViewer::month_changed, Qt::QueuedConnection);
    connect(ui->year_edit, &QDateEdit::dateChanged, this, &DiaryEntryViewer::year_changed, Qt::QueuedConnection);
    connect(ui->next_month, &QPushButton::clicked, this, &DiaryEntryViewer::next_month, Qt::QueuedConnection);
    connect(ui->prev_month, &QPushButton::clicked, this, &DiaryEntryViewer::prev_month, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryEntryViewer::change_month,
        Qt::QueuedConnection);
    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEntryViewer::update_theme,
        Qt::QueuedConnection);
    update_theme();

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryEntryViewer::~DiaryEntryViewer()
{
    delete ui;
}

void DiaryEntryViewer::update_theme()
{
    change_month(QDate::currentDate());
}

void DiaryEntryViewer::change_month(QDate const &date)
{
    // Update the selector UI.
    ui->month_dropdown->blockSignals(true);
    ui->year_edit->blockSignals(true);

    if (date.isValid()) {
        ui->month_dropdown->setCurrentIndex(date.month() - 1);
        ui->year_edit->setDate(date);
    }
    else {
        ui->month_dropdown->setCurrentIndex(current_date.month() - 1);
        ui->year_edit->setDate(current_date);
    }

    ui->month_dropdown->blockSignals(false);
    ui->year_edit->blockSignals(false);

    auto const &opt = DiaryHolder::instance()->get_monthmap(date.isValid() ? date : current_date);
    if (!opt)
        return ui->entry_edit->setText(PLACEHOLDER_TEXT);

    QString html(TABLE_START);

    auto row_counter = 0;
    for (auto const &i : (*opt)->second) {
        auto const &[important, rating, message, dummy] = i.second;

        // Don't add any days that don't have text entries.
        if (message.empty())
            continue;

        auto copy = message;

        // This is just about the most janky thing in this entire app.
        html.append(QString(SINGLE_ROW)
                        .arg(generate_base64_icon(i.first, rating, important), misc::sanitise_html(misc::trim(copy))));
        html.append(HR_ROW);

        ++row_counter;
    }

    if (0 == row_counter) {
        return ui->entry_edit->setText(PLACEHOLDER_TEXT);
    }
    else {
        html.chop(HR_ROW.size());
        html.append(TABLE_END);
        ui->entry_edit->setText(html);
    }

    current_date = date;

    qDebug() << "Changed diary entry viewer month:" << date;
}

void DiaryEntryViewer::next_month()
{
    QDate const next = ui->year_edit->date().addMonths(1);
    if (next.isValid())
        change_month(next);
}

void DiaryEntryViewer::prev_month()
{
    QDate const prev = ui->year_edit->date().addMonths(-1);
    if (prev.isValid())
        change_month(prev);
}

void DiaryEntryViewer::month_changed(int)
{
    change_month(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryEntryViewer::year_changed(QDate const &date)
{
    if (date.isValid())
        change_month(QDate(ui->year_edit->date().year(), ui->month_dropdown->currentIndex() + 1, 1));
}

QByteArray DiaryEntryViewer::generate_base64_icon(int const day, td::Rating const rating, bool const important)
{
    QPixmap pixmap(SIZE, SIZE);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawPixmap(0, 0, generate_background(rating));

    if (important)
        p.drawPixmap(0, 0, generate_star(rating));

    p.drawPixmap(0, 0, generate_text(day, rating));

    QByteArray barray;
    QBuffer buf(&barray);
    buf.open(QIODevice::WriteOnly);

    pixmap.toImage().save(&buf, "PNG");
    return barray.toBase64();
}

QPixmap DiaryEntryViewer::generate_background(td::Rating const rating)
{
    QString key = QString("diaryentryicon:bkg:%1:%2")
                      .arg(QString::number(static_cast<int>(rating)),
                          QString::number(static_cast<int>(InternalManager::instance()->get_theme())));
    QPixmap pixmap;

    if (!QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(SIZE, SIZE);
        pixmap.fill(Qt::transparent);

        QColor colour = misc::rating_to_colour(rating);
        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);

        if (InternalManager::instance()->get_theme() == td::Theme::Light)
            p.setOpacity(0.8);

        p.setPen(Qt::transparent);
        p.setBrush(QBrush(colour));
        p.drawEllipse(0, 0, SIZE, SIZE);

        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}

QPixmap DiaryEntryViewer::generate_star(td::Rating const rating)
{
    auto theme_str = misc::rating_to_theme(rating) == td::Theme::Dark ? "dark" : "light";
    QString key = QString("diaryentryicon:star:%1").arg(theme_str);
    QPixmap pixmap;

    if (!QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(SIZE, SIZE);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(0.3);

        auto overlay = QIcon(QString(":/themes/%1/star.svg").arg(theme_str)).pixmap(SIZE * 0.8, SIZE * 0.8);

        // Draw overlay on the centre of the pixmap.
        auto x = ((rect().bottomRight().x() - overlay.rect().bottomRight().x()) / 2);
        // y is the same as x since it's a square.

        p.drawPixmap(x, x, overlay);

        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}

QPixmap DiaryEntryViewer::generate_text(int const day_, td::Rating const rating)
{
    QString day = QString::number(day_);
    auto theme = QString::number(static_cast<int>(misc::rating_to_theme(rating)));
    QString key = QString("diaryentryicon:text:%1:%2").arg(day, theme);
    QPixmap pixmap;

    if (!QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(SIZE, SIZE);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);

        auto bold_font = QApplication::font();
        bold_font.setBold(true);
        bold_font.setPointSize(16);
        p.setFont(bold_font);

        QRectF rect;
        rect = p.boundingRect(pixmap.rect(), Qt::AlignCenter, day);

        p.setPen(misc::theme_to_text(misc::rating_to_theme(rating)));
        p.drawText(rect, Qt::AlignCenter, day);

        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}
