#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  // This is to fix fonts not scaling properly at different DPI
  // https://stackoverflow.com/a/36058882
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
