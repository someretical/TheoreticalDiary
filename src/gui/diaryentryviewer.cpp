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

// windef.h already defined SIZE >:(
// Thanks Microsoft for being the outlier in conformance AGAIN.
namespace override {
int const SIZE = 50;
}

const char *PLACEHOLDER_TEXT = R"(
<center><p>It seems there are no entries yet for this month...</p></center>
)";

char const *TABLE_START = "<table border=0 cellspacing=15>";

char const *SINGLE_ROW = R"(
<tr>
<td valign=top><img src="data:image/png;base64, %1" alt="" width="50" height="50"/></td>
<td valign=middle title="%2"><font size="5">%3</font></td>
</tr>
)";

const QString HR_ROW(R"(
<tr>
<td></td>
<td><hr></td>
</tr>
)");

char const *TABLE_END = "</table>";

DiaryEntryViewer::DiaryEntryViewer(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryEntryViewer)
{
    m_ui->setupUi(this);

    // Navigator slots
    connect(m_ui->month_dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &DiaryEntryViewer::month_changed, Qt::QueuedConnection);
    connect(m_ui->year_edit, &QDateEdit::dateChanged, this, &DiaryEntryViewer::year_changed, Qt::QueuedConnection);
    connect(m_ui->next_month, &QPushButton::clicked, this, &DiaryEntryViewer::next_month, Qt::QueuedConnection);
    connect(m_ui->prev_month, &QPushButton::clicked, this, &DiaryEntryViewer::prev_month, Qt::QueuedConnection);

    // Next/prev month keyboard shortcuts
    m_next_month_bind = new QShortcut(Qt::Key_Down, this);
    m_next_month_bind->setAutoRepeat(true);
    connect(m_next_month_bind, &QShortcut::activated, this, &DiaryEntryViewer::next_month, Qt::QueuedConnection);
    m_prev_month_bind = new QShortcut(Qt::Key_Up, this);
    m_prev_month_bind->setAutoRepeat(true);
    connect(m_prev_month_bind, &QShortcut::activated, this, &DiaryEntryViewer::prev_month, Qt::QueuedConnection);

    connect(InternalManager::instance(), &InternalManager::update_data, this, &DiaryEntryViewer::change_month,
        Qt::QueuedConnection);
    connect(InternalManager::instance(), &InternalManager::update_theme, this, &DiaryEntryViewer::update_theme,
        Qt::QueuedConnection);

    // current_date is initialised by &InternalManager::change_month signal.
}

DiaryEntryViewer::~DiaryEntryViewer()
{
    delete m_ui;
    delete m_next_month_bind;
    delete m_prev_month_bind;
}

void DiaryEntryViewer::update_theme()
{
    change_month(QDate::currentDate());
}

void DiaryEntryViewer::change_month(QDate const &date)
{
    qDebug() << "Changed diary entry viewer month:" << date;

    // Update the selector UI.
    const QSignalBlocker b1(m_ui->month_dropdown);
    const QSignalBlocker b2(m_ui->year_edit);

    if (date.isValid()) {
        m_ui->month_dropdown->setCurrentIndex(date.month() - 1);
        m_ui->year_edit->setDate(date);
    }
    else {
        m_ui->month_dropdown->setCurrentIndex(m_current_date.month() - 1);
        m_ui->year_edit->setDate(m_current_date);
    }

    auto const &opt = DiaryHolder::instance()->get_monthmap(date.isValid() ? date : m_current_date);
    if (!opt)
        return m_ui->entry_edit->setText(PLACEHOLDER_TEXT);

    QString html(TABLE_START);

    auto row_counter = 0;
    for (auto const &i : (*opt)->second) {
        auto const &[important, rating, general_message, dummy, d2, le] = i.second;

        // Don't add any days that don't have general_message's.
        if (general_message.empty())
            continue;

        auto copy = general_message;
        QDateTime last_edited;
        last_edited.setTime_t(le);

        // This is just about the most janky thing in this entire app.
        // I am dynamically generating a PNG image file and converting it to base64 to embed in a HTML file LOL.
        html.append(
            QString(SINGLE_ROW)
                .arg(generate_base64_icon(i.first, rating, important),
                    last_edited.toString("dddd MMMM d%1 yyyy 'at' h:mm:ss ap").arg(misc::get_day_suffix(i.first)),
                    misc::sanitise_html(misc::trim(copy))));
        //        html.append(
        //            QString(SINGLE_ROW)
        //                .arg(generate_base64_icon(i.first, rating, important),
        //                    last_edited.toString("dddd MMMM d%1 yyyy 'at' h:mm:ss
        //                    ap").arg(misc::get_day_suffix(i.first)), "PLACEHOLDER"));
        html.append(HR_ROW);

        ++row_counter;
    }

    if (0 == row_counter) {
        return m_ui->entry_edit->setText(PLACEHOLDER_TEXT);
    }
    else {
        html.chop(HR_ROW.size());
        html.append(TABLE_END);
        m_ui->entry_edit->setText(html);
    }

    m_current_date = date;
}

void DiaryEntryViewer::next_month()
{
    QDate const next = m_ui->year_edit->date().addMonths(1);
    if (next.isValid())
        change_month(next);
}

void DiaryEntryViewer::prev_month()
{
    QDate const prev = m_ui->year_edit->date().addMonths(-1);
    if (prev.isValid())
        change_month(prev);
}

void DiaryEntryViewer::month_changed(int)
{
    change_month(QDate(m_ui->year_edit->date().year(), m_ui->month_dropdown->currentIndex() + 1, 1));
}

void DiaryEntryViewer::year_changed(QDate const &date)
{
    if (date.isValid())
        change_month(QDate(m_ui->year_edit->date().year(), m_ui->month_dropdown->currentIndex() + 1, 1));
}

QByteArray DiaryEntryViewer::generate_base64_icon(int const day, td::Rating const rating, bool const important)
{
    QPixmap pixmap(override::SIZE, override::SIZE);
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
        pixmap = QPixmap(override::SIZE, override::SIZE);
        pixmap.fill(Qt::transparent);

        QColor colour = misc::rating_to_colour(rating);
        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);

        //        if (InternalManager::instance()->get_theme() == td::Theme::Light)
        //            p.setOpacity(0.8);

        p.setPen(Qt::transparent);
        p.setBrush(QBrush(colour));
        p.drawEllipse(0, 0, override::SIZE, override::SIZE);

        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}

QPixmap DiaryEntryViewer::generate_star(td::Rating const rating)
{
    auto theme_str = misc::rating_to_theme(rating) == td::Theme::Dark ? "dark" : "light";
    QString key = QString("diaryentryicon:star:%1").arg(theme_str);
    QPixmap pixmap(override::SIZE, override::SIZE);

    if (!QPixmapCache::find(key, pixmap)) {
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);
        p.setOpacity(0.2);

        QSvgRenderer renderer(QString(":/themes/%1/star.svg").arg(theme_str));
        QPixmap star(override::SIZE * 0.8, override::SIZE * 0.8);
        star.fill(Qt::transparent);
        QPainter star_painter(&star);
        renderer.render(&star_painter);

        // Draw overlay on the centre of the pixmap.
        auto x = ((override::SIZE - star.rect().bottomRight().x()) / 2);
        // y is the same as x since it's a square.

        p.drawPixmap(x, x, star);

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
        pixmap = QPixmap(override::SIZE, override::SIZE);
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
