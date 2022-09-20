/*
 *  Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
 *  Copyright (C) 2011 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/Icons.h"
#include "gui/MainWindow.h"

#include <Logger.h>
#include <QBuffer>
#include <QIconEngine>
#include <QImageReader>
#include <QPaintDevice>
#include <QPainter>
#include <QPalette>

class AdaptiveIconEngine : public QIconEngine {
public:
    explicit AdaptiveIconEngine(QIcon baseIcon, QColor overrideColor = {});
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    auto pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) -> QPixmap override;
    [[nodiscard]] auto clone() const -> QIconEngine * override;

private:
    QIcon m_baseIcon;
    QColor m_overrideColor;
};

AdaptiveIconEngine::AdaptiveIconEngine(QIcon baseIcon, QColor overrideColor)
    : QIconEngine(), m_baseIcon(std::move(baseIcon)), m_overrideColor(std::move(overrideColor))
{
}

void AdaptiveIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    // Temporary image canvas to ensure that the background is transparent and alpha blending works.
    auto scale = painter->device()->devicePixelRatioF();
    QImage img(rect.size() * scale, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);

    m_baseIcon.paint(&p, img.rect(), Qt::AlignCenter, mode, state);

    if (m_overrideColor.isValid()) {
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(img.rect(), m_overrideColor);
    }
    else if (mainWindow()) {
        QPalette palette = mainWindow()->palette();
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);

        switch (mode) {
        case QIcon::Active:
            p.fillRect(img.rect(), palette.color(QPalette::Active, QPalette::ButtonText));
            break;
        case QIcon::Selected:
            p.fillRect(img.rect(), palette.color(QPalette::Active, QPalette::HighlightedText));
            break;
        case QIcon::Disabled:
            p.fillRect(img.rect(), palette.color(QPalette::Disabled, QPalette::WindowText));
            break;
        default:
            p.fillRect(img.rect(), palette.color(QPalette::Normal, QPalette::WindowText));
            break;
        }
    }

    painter->drawImage(rect, img);
}

auto AdaptiveIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) -> QPixmap
{
    QImage img(size, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    QPainter painter(&img);
    paint(&painter, QRect(0, 0, size.width(), size.height()), mode, state);
    return QPixmap::fromImage(img, Qt::ImageConversionFlag::NoFormatConversion);
}

auto AdaptiveIconEngine::clone() const -> QIconEngine *
{
    return new AdaptiveIconEngine(m_baseIcon);
}

Icons::Icons()
{
    QIcon::setThemeSearchPaths(QStringList{":/icons"} << QIcon::themeSearchPaths());
    QIcon::setThemeName("application");
}

Icons::~Icons() = default;

auto Icons::icon(const QString &name, bool recolor, const QColor &overrideColor) -> QIcon
{
#ifdef Q_OS_LINUX
    // Resetting the application theme name before calling QIcon::fromTheme() is required for hacky
    // QPA platform themes such as qt5ct, which randomly mess with the configured icon theme.
    // If we do not reset the theme name here, it will become empty at some point, causing
    // Qt to look for icons at the user-level and global default locations.
    //
    // See issue #4963: https://github.com/keepassxreboot/keepassxc/issues/4963
    // and qt5ct issue #80: https://sourceforge.net/p/qt5ct/tickets/80/
    QIcon::setThemeName("application");
#endif

    QString cacheName =
        QString("%1:%2:%3").arg(recolor ? "1" : "0", overrideColor.isValid() ? overrideColor.name() : "#", name);
    QIcon icon = m_iconCache.value(cacheName);

    if (!icon.isNull() && !overrideColor.isValid()) {
        return icon;
    }

    icon = QIcon::fromTheme(name);
    if (recolor) {
        icon = QIcon(new AdaptiveIconEngine(icon, overrideColor));
        icon.setIsMask(true);
    }

    m_iconCache.insert(cacheName, icon);
    return icon;
}

auto Icons::onOffIcon(const QString &name, bool on, bool recolor) -> QIcon
{
    return icon(name + (on ? "-on" : "-off"), recolor);
}
