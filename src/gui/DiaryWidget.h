//
// Created by someretical on 6/08/22.
//

#ifndef THEORETICAL_DIARY_DIARYWIDGET_H
#define THEORETICAL_DIARY_DIARYWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DiaryWidget;
}
QT_END_NAMESPACE

class DiaryWidget : public QWidget {
    Q_OBJECT

public:
    explicit DiaryWidget(QWidget *parent = nullptr);
    ~DiaryWidget() override;

private:
    Ui::DiaryWidget *ui;
};

#endif // THEORETICAL_DIARY_DIARYWIDGET_H
