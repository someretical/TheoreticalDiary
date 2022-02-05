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

#include <QtSvg>
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
    void resize_slot(int const new_width)
    {
        setFixedHeight(new_width);
        setFixedWidth(new_width);

        update();
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        auto const &size_ = size();

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawPixmap(0, 0, generate_pixmap(size_));

        if (special) {
            p.setOpacity(0.5);
            p.drawPixmap(0, 0, generate_star(size_));
        }
    }

private:
    QPixmap generate_pixmap(QSize const &size_)
    {
        QPixmap pixmap(size_.width(), size_.width());
        QString key = QString("pixellabel:%1:%2:%3")
                          .arg(QString::number(static_cast<int>(rating)), QString::number(size_.width()),
                              QString::number(static_cast<int>(InternalManager::instance()->get_theme())));

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap.fill(Qt::transparent);
            QColor bkg_colour = misc::rating_to_colour(rating);
            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing);

            //        if (InternalManager::instance()->get_theme() == td::Theme::Light)
            //            p.setOpacity(0.8);

            p.setPen(bkg_colour);
            p.setBrush(QBrush(bkg_colour));
            p.drawRoundedRect(0, 0, size_.width(), size_.width(), 5, 5);

            QPixmapCache::insert(key, pixmap);
        }

        return pixmap;
    }

    QPixmap generate_star(QSize const &size_)
    {
        QPixmap pixmap(size_.width(), size_.width());
        pixmap.fill(Qt::transparent);

        auto theme_str = misc::rating_to_theme(rating) == td::Theme::Dark ? "dark" : "light";
        QSvgRenderer renderer(QString(":/themes/%1/star.svg").arg(theme_str));
        QPainter painter(&pixmap);
        renderer.render(&painter);

        return pixmap;
    }
};

#endif // DIARYPIXELLABEL_H
