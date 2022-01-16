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

#ifndef DIARYCOMPARISONLABEL_H
#define DIARYCOMPARISONLABEL_H

#include <QtWidgets>

#include "../core/internalmanager.h"
#include "misc.h"

// Not available to other files.
static int const SIZE = 50;

class DiaryComparisonLabel : public QLabel {
    Q_OBJECT

public:
    DiaryComparisonLabel(QWidget *parent = nullptr) : QLabel(parent)
    {
        setFixedSize(QSize(50, 50));
        rating = td::Rating::Unknown;
        special = false;
    }

    ~DiaryComparisonLabel() {}

    td::Rating rating;
    bool special;

private:
    QPixmap generate_pixmap()
    {
        if (special) {
            auto theme_str = InternalManager::instance()->get_theme_str(true);
            auto svg = QIcon(QString(":/themes/%1/star.svg").arg(theme_str)).pixmap(size());

            QPixmap bkg(SIZE, SIZE);
            bkg.fill(Qt::transparent);
            QPainter p(&bkg);
            p.setRenderHint(QPainter::Antialiasing);
            p.setOpacity(0.5);
            p.drawPixmap(0, 0, svg);
            return bkg;
        }
        else {
            QPixmap bkg(SIZE, SIZE);
            bkg.fill(Qt::transparent);

            QColor bkg_color = misc::rating_to_colour(rating);
            QPainter p(&bkg);
            p.setRenderHint(QPainter::Antialiasing);

            if (InternalManager::instance()->get_theme() == td::Theme::Light)
                p.setOpacity(0.8);

            p.setPen(bkg_color);
            p.setBrush(QBrush(bkg_color));
            p.drawRoundedRect(0, 0, SIZE, SIZE, 5, 5);

            return bkg;
        }
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        QString key =
            QString("comparisonlabel:%1:%2")
                .arg(QString::number(static_cast<int>(rating)),
                    special ? "s" : QString::number(static_cast<int>(InternalManager::instance()->get_theme())));
        QPixmap pixmap;

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap = generate_pixmap();
            QPixmapCache::insert(key, pixmap);
        }

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawPixmap(0, 0, pixmap);
    }
};

#endif // DIARYCOMPARISONLABEL_H
