/*
 * Copyright (C) 2017 KeePassXC Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or (at your option)
 * version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSXC_GUI_CATEGORYLISTWIDGET_H
#define KEEPASSXC_GUI_CATEGORYLISTWIDGET_H

#include <QPointer>
#include <QStyledItemDelegate>

class CategoryListWidgetDelegate;
class QListWidget;

namespace Ui {
class CategoryListWidget;
}

class CategoryListWidget : public QWidget {
    Q_OBJECT

public:
    explicit CategoryListWidget(QWidget *parent = nullptr);
    ~CategoryListWidget() override;

    auto currentCategory() -> int;
    void setCurrentCategory(int index);
    auto addCategory(const QString &labelText, const QIcon &icon) -> int;
    void setCategoryHidden(int index, bool hidden);
    auto isCategoryHidden(int index) -> bool;
    void removeCategory(int index);

signals:
    void categoryChanged(int index);

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    auto sizeHint() const -> QSize override;
    auto minimumSizeHint() const -> QSize override;

protected slots:
    void updateCategoryScrollButtons();
    void scrollCategoriesDown();
    void scrollCategoriesUp();
    void emitCategoryChanged(int index);

private:
    QPointer<CategoryListWidgetDelegate> m_itemDelegate;
    const QScopedPointer<Ui::CategoryListWidget> m_ui;

    Q_DISABLE_COPY(CategoryListWidget)
};

/* =============================================================================================== */

class CategoryListWidgetDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit CategoryListWidgetDelegate(QListWidget *parent = nullptr);
    auto minWidth() const -> int;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    auto sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const -> QSize override;

private:
    const int ICON_SIZE = 32;

    QPointer<QListWidget> m_listWidget;
    QSize m_size;

    Q_DISABLE_COPY(CategoryListWidgetDelegate)
};

#endif
