#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup normal buttons
    connect(ui->open_button, SIGNAL(clicked()), this, SLOT(open_button_pressed()));
    connect(ui->new_button, SIGNAL(clicked()), this, SLOT(new_button_pressed()));
    connect(ui->dl_button, SIGNAL(clicked()), this, SLOT(dl_button_pressed()));
    connect(ui->import_button, SIGNAL(clicked()), this, SLOT(import_button_pressed()));
    connect(ui->flush_button, SIGNAL(clicked()), this, SLOT(flush_button_pressed()));
    connect(ui->dump_button, SIGNAL(clicked()), this, SLOT(dump_button_pressed()));

    // Setup expandable options
    ui->options->hide();
    connect(ui->options_button, SIGNAL(clicked()), this, SLOT(toggle_advanced_options()));

    // Setup quit action
    auto action_quit = this->findChild<QAction *>("action_quit");
    addAction(action_quit);
    connect(action_quit, SIGNAL(triggered()), this, SLOT(quit_app()), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_button_pressed()
{

}

void MainWindow::new_button_pressed() {

}

void MainWindow::dl_button_pressed(){

}

void MainWindow::import_button_pressed(){

}

void MainWindow::flush_button_pressed(){

}

void MainWindow::dump_button_pressed(){

}

void MainWindow::toggle_advanced_options() {
    if (ui->options->isVisible())
        ui->options->hide();
    else ui->options->show();
}

void MainWindow::quit_app() {
    qApp->quit();
}

