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

#ifndef DIARYCALENDARBUTTON_H
#define DIARYCALENDARBUTTON_H

class DiaryEditor;

#include <QtSvg>
#include <QtWidgets>
#include <optional>

#include "../core/internalmanager.h"
#include "misc.h"

// windef.h already defined SIZE >:(
// Thanks Microsoft for being the outlier in conformance AGAIN.
namespace override {
static int const SIZE = 86; // Should be even.
}

static int const BORDER_WIDTH = 4; // Should be even.
static int const ROUNDNESS = 8;    // Should be even.

class DiaryCalendarButton : public QAbstractButton {
    Q_OBJECT

signals:
    void sig_clicked(int const day);

public:
    DiaryCalendarButton(td::CalendarButtonData const &d, QWidget *p = nullptr) : QAbstractButton(p)
    {
        setText(QString::number(*d.day));
        setFixedSize(QSize(override::SIZE, override::SIZE));
        m_data = d;
        m_m_down = false;
    }

    ~DiaryCalendarButton() {}

    td::CalendarButtonData m_data;
    bool m_m_down;

public slots:
    void render(td::CalendarButtonData const &d)
    {
        if (d.day)
            m_data.day = d.day;

        if (d.important)
            m_data.important = d.important;

        if (d.rating)
            m_data.rating = d.rating;

        if (d.selected)
            m_data.selected = d.selected;

        if (d.current_day)
            m_data.current_day = d.current_day;

        update();
    }

protected:
    void enterEvent(QEvent *)
    {
        update();
    }

    void leaveEvent(QEvent *)
    {
        update();
    }

    void mousePressEvent(QMouseEvent *)
    {
        m_m_down = true;
        update();
    }

    void mouseReleaseEvent(QMouseEvent *e)
    {
        m_m_down = false;

        // Mouse move events are not broadcast when the mouse is held down.
        if (rect().contains(e->pos()))
            emit sig_clicked(*m_data.day);

        update();
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawPixmap(0, 0, generate_background());

        if (*m_data.important)
            p.drawPixmap(0, 0, generate_star());

        p.drawPixmap(0, 0, generate_text());
    }

private:
    QPixmap generate_background()
    {
        QString key = QString("calendarbutton:bkg:%1:%2:%3")
                          .arg(QString::number(static_cast<int>(*m_data.rating)),
                              QString::number(static_cast<int>(InternalManager::instance()->get_theme())),
                              QString::number(get_state()));
        QPixmap pixmap;

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap = QPixmap(override::SIZE, override::SIZE);
            pixmap.fill(Qt::transparent);

            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing);

            QPainterPath path;
            auto topleft_offset = BORDER_WIDTH / 2;
            auto adjusted_size = override::SIZE - BORDER_WIDTH;
            path.addRoundedRect(
                QRect(topleft_offset, topleft_offset, adjusted_size, adjusted_size), ROUNDNESS, ROUNDNESS);

            //            if (InternalManager::instance()->get_theme() == td::Theme::Light)
            //                p.setOpacity(0.8);

            // Set background color.
            QColor color = misc::rating_to_colour(*m_data.rating);
            if (m_m_down) {
                p.fillPath(path, QBrush(color.darker(120)));
            }
            //            else if (underMouse()) {
            //                p.fillPath(path, QBrush(color.lighter(110)));
            //            }
            else {
                p.fillPath(path, QBrush(color));
            }

            // Set border color.
            if (m_m_down || underMouse() || *m_data.selected) {
                if (InternalManager::instance()->get_theme() == td::Theme::Light) {
                    p.strokePath(path, QPen(QBrush(color.lighter(110)), BORDER_WIDTH));
                }
                else {
                    p.strokePath(path, QPen(QBrush(color.darker(130)), BORDER_WIDTH));
                }
            }
            else {
                p.setCompositionMode(QPainter::CompositionMode_Clear); // This will actually erase.
                p.strokePath(path, QPen(QBrush(Qt::transparent), BORDER_WIDTH));
            }

            QPixmapCache::insert(key, pixmap);
        }

        return pixmap;
    }

    QPixmap generate_text()
    {
        QString day = QString::number(*m_data.day);
        QPixmap pixmap;

        if (*m_data.current_day) {

            pixmap = QPixmap(override::SIZE, override::SIZE);
            pixmap.fill(Qt::transparent);

            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing);

            auto bold_font = QApplication::font();
            bold_font.setBold(true);
            bold_font.setItalic(true);
            bold_font.setPointSize(16);
            p.setFont(bold_font);

            QRectF rect;
            rect = p.boundingRect(pixmap.rect(), Qt::AlignTop | Qt::AlignRight, day);
            rect.translate(-12, 8);

            p.setPen(misc::theme_to_text(misc::rating_to_theme(*m_data.rating)));
            p.drawText(rect, Qt::AlignTop | Qt::AlignRight, day);

            return pixmap;
        }

        auto theme = QString::number(static_cast<int>(misc::rating_to_theme(*m_data.rating)));
        QString key = QString("calendarbutton:text:%1:%2").arg(day, theme);

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap = QPixmap(override::SIZE, override::SIZE);
            pixmap.fill(Qt::transparent);

            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing);

            auto bold_font = QApplication::font();
            bold_font.setBold(true);
            bold_font.setPointSize(16);
            QRectF rect;

            p.setFont(bold_font);
            rect = p.boundingRect(pixmap.rect(), Qt::AlignTop | Qt::AlignRight, day);
            rect.translate(-10, 8);

            p.setPen(misc::theme_to_text(misc::rating_to_theme(*m_data.rating)));
            p.drawText(rect, Qt::AlignTop | Qt::AlignRight, day);

            QPixmapCache::insert(key, pixmap);
        }

        return pixmap;
    }

    QPixmap generate_star()
    {
        auto theme_str = misc::rating_to_theme(*m_data.rating) == td::Theme::Dark ? "dark" : "light";
        QString key = QString("calendarbutton:star:%1:%2").arg(theme_str, QString::number(get_state()));
        QPixmap pixmap(override::SIZE, override::SIZE);

        if (!QPixmapCache::find(key, pixmap)) {
            pixmap.fill(Qt::transparent);

            QPainter p(&pixmap);
            p.setRenderHint(QPainter::Antialiasing);

            if (m_m_down) {
                p.setOpacity(0.4);
            }
            else if (underMouse()) {
                p.setOpacity(0.4);
            }
            else {
                p.setOpacity(0.5);
            }

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

    int get_state()
    {
        // If the mouse is NOT hovering over and NOT clicking, state is 0.
        // If the mouse IS hovering over but not clicking, state is 1.
        // If the mouse IS hovering over AND clicking, state is 2.

        if (m_m_down)
            return 2;

        if (underMouse() || *m_data.selected)
            return 1;

        return 0;
    }
};

#endif // DIARYCALENDARBUTTON_H
