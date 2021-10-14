#include "confirmoverwrite.h"
#include "mainwindow.h"
#include "ui_confirmoverwrite.h"

#include <QFile>

ConfirmOverwrite::ConfirmOverwrite(void (MainWindow::*slot)(const int),
                                   QWidget *parent)
    : QDialog(parent), ui(new Ui::ConfirmOverwrite) {
  ui->setupUi(this);

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  auto action = findChild<QAction *>("action_no");
  addAction(action);
  connect(action, &QAction::triggered, this, &ConfirmOverwrite::action_no,
          Qt::QueuedConnection);

  action = findChild<QAction *>("action_yes");
  addAction(action);
  connect(action, &QAction::triggered, this, &ConfirmOverwrite::action_yes,
          Qt::QueuedConnection);

  connect(this, &ConfirmOverwrite::sig_complete,
          qobject_cast<MainWindow *>(parent), slot);
}

ConfirmOverwrite::~ConfirmOverwrite() { delete ui; }

void ConfirmOverwrite::action_no() {
  emit sig_complete(1);
  accept();
}

void ConfirmOverwrite::action_yes() {
  emit sig_complete(0);
  accept();
}
