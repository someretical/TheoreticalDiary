//
// Created by someretical on 6/08/22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DiaryWidget.h" resolved

#include "DiaryWidget.h"
#include "ui_DiaryWidget.h"

DiaryWidget::DiaryWidget(QWidget *parent) : QWidget(parent), ui(new Ui::DiaryWidget)
{
    ui->setupUi(this);
}

DiaryWidget::~DiaryWidget()
{
    delete ui;
}
