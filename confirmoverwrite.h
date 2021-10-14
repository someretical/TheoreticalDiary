#ifndef CONFIRMOVERWRITE_H
#define CONFIRMOVERWRITE_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class ConfirmOverwrite;
}

class ConfirmOverwrite : public QDialog {
  Q_OBJECT

signals:
  void sig_complete(const int code);

public:
  /**
   * So function pointers can never be null.
   * That means they don't need to and can't be passed as references.
   * Also function pointers aren't actually pointers :^)
   * The more you know...
   */
  explicit ConfirmOverwrite(void (MainWindow::*slot)(const int),
                            QWidget *parent = nullptr);
  ~ConfirmOverwrite();
  void action_no();
  void action_yes();

private:
  Ui::ConfirmOverwrite *ui;
};

#endif // CONFIRMOVERWRITE_H
