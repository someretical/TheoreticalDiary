#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "gui/Font.h"
#include "qmarkdowntextedit.h"

#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QWidget>

class LineNumArea final : public QWidget {
    Q_OBJECT

public:
    explicit LineNumArea(QMarkdownTextEdit *parent)
        : QWidget(parent), p_textEdit(parent)
    {
        Q_ASSERT(parent);

        setContentsMargins(0, 0, 0, 0);

        m_currentLineColour = QColor(QStringLiteral("#eef067"));
        m_otherLineColour = QColor(QStringLiteral("#a6a6a6"));
        setHidden(true);

        // We always use fixed font to avoid "width" issues
        auto f = Font::fixedFont();
        f.setPointSize(f.pointSize() - 3);
        setFont(f);
    }

    void setCurrentLineColor(QColor color) {
        m_currentLineColour = std::move(color);
    }

    void setOtherLineColor(QColor color) {
        m_otherLineColour = std::move(color);
    }

    [[nodiscard]] auto lineNumAreaWidth() const -> int
    {
        if (!m_enabled) {
            return 0;
        }

        int digits = 1;
        int max = std::max(1, p_textEdit->blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }

#if QT_VERSION >= 0x050B00
        int space = 4 + QFontMetrics(font()).horizontalAdvance(u'9') * digits;
#else
        int space = 4 + QFontMetrics(font()).width(QLatin1Char('9')) * digits;
#endif

        return space;
    }

    [[nodiscard]] auto isLineNumAreaEnabled() const -> bool
    {
        return m_enabled;
    }

    void setLineNumAreaEnabled(bool e)
    {
        m_enabled = e;
        setHidden(!e);
    }

    [[nodiscard]] auto sizeHint() const -> QSize override
    {
        return {lineNumAreaWidth(), 0};
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);

        auto block = p_textEdit->firstVisibleBlock();
        int blockNumber = block.blockNumber();
        qreal top = p_textEdit->blockBoundingGeometry(block).translated(p_textEdit->contentOffset()).top();
        // Maybe the top is not 0?
        top += p_textEdit->viewportMargins().top();
        qreal bottom = top;

        const QPen currentLine = m_currentLineColour;
        const QPen otherLines = m_otherLineColour;
        painter.setFont(font());

        while (block.isValid() && top <= event->rect().bottom()) {
            top = bottom;
            bottom = top + p_textEdit->blockBoundingRect(block).height();
            if (block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);

                auto isCurrentLine = p_textEdit->textCursor().blockNumber() == blockNumber;
                painter.setPen(isCurrentLine ? currentLine : otherLines);

                painter.drawText(
                    -5,
                    top,
                    sizeHint().width(), p_textEdit->fontMetrics().height(),
                    Qt::AlignRight | Qt::AlignVCenter,
                    number
                );
            }

            block = block.next();
            ++blockNumber;
        }
    }

private:
    bool m_enabled = false;
    QMarkdownTextEdit *p_textEdit;
    QColor m_currentLineColour;
    QColor m_otherLineColour;
};

#endif // LINENUMBERAREA_H
