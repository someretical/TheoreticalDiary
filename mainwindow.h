#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void open_button_pressed();
    void new_button_pressed();
    void dl_button_pressed();
    void import_button_pressed();
    void flush_button_pressed();
    void dump_button_pressed();
    void toggle_advanced_options();
    void quit_app();
};
#endif // MAINWINDOW_H
