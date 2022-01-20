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

#ifndef DIARYPIXELLABEL_H
#define DIARYPIXELLABEL_H

#include <QtWidgets>

#include "../core/internalmanager.h"
#include "misc.h"

namespace override {
static char const *MONTHS[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
    "October", "November", "December"};
}

class DiaryPixelLabel : public QLabel {
    Q_OBJECT

public:
    explicit DiaryPixelLabel(
        td::Rating const r, bool const s, int const month, int const day, int const size, QWidget *parent = nullptr)
        : QLabel(parent)
    {
        special = s;
        rating = r;

        setFixedHeight(size);
        setFixedWidth(size);

        setToolTip(
            QString("%1 %2%3").arg(override::MONTHS[month - 1], QString::number(day), misc::get_day_suffix(day)));
    }

    ~DiaryPixelLabel() {}

    td::Rating rating;
    bool special;

public slots:
    void resize(int const new_width)
    {
        setFixedHeight(new_width);
        setFixedWidth(new_width);
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        auto const &size_ = size();

        QString key = QString("pixellabel:%1:%2:%3")
                          .arg(QString::number(static_cast<int>(rating)), QString::number(size_.width()),
                              QString::number(static_cast<int>(InternalManager::instance()->get_theme())));
        QPixmap pixmap;

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap = generate_pixmap(size_);
            QPixmapCache::insert(key, pixmap);
        }

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawPixmap(0, 0, pixmap);

        // Since starred days are supposed to be a lot less common, caching isn't done for them.
        if (special) {
            auto theme_str = misc::rating_to_theme(rating) == td::Theme::Dark ? "dark" : "light";
            QPixmap svg_overlay = QIcon(QString(":/themes/%1/star.svg").arg(theme_str)).pixmap(size_);

            p.setOpacity(0.5);
            p.drawPixmap(0, 0, svg_overlay);
        }
    }

private:
    QPixmap generate_pixmap(QSize const &size_)
    {
        QPixmap bkg(size_.width(), size_.width());
        bkg.fill(Qt::transparent);
        QColor bkg_colour = misc::rating_to_colour(rating);
        QPainter p(&bkg);
        p.setRenderHint(QPainter::Antialiasing);

        //        if (InternalManager::instance()->get_theme() == td::Theme::Light)
        //            p.setOpacity(0.8);

        p.setPen(bkg_colour);
        p.setBrush(QBrush(bkg_colour));
        p.drawRoundedRect(0, 0, size_.width(), size_.width(), 5, 5);

        return bkg;
    }
};

#endif // DIARYPIXELLABEL_H
