#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->options->hide();
    connect(ui->options_button, SIGNAL(clicked()), this, SLOT(toggle()));
}

void MainWindow::toggle() {
    if (ui->options->isVisible())
        ui->options->hide();
    else ui->options->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

