#include "promptauth.h"
#include "googlewrapper.h"
#include "theoreticaldiary.h"
#include "ui_promptauth.h"

PromptAuth::PromptAuth(QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptAuth) {
  ui->setupUi(this);

  QFile ss_file(":/styles/defaultwindow.qss");
  ss_file.open(QIODevice::ReadOnly);
  QString stylesheet = ss_file.readAll();
  setStyleSheet(stylesheet);

  connect(TheoreticalDiary::instance()->gwrapper,
          &GoogleWrapper::sig_oauth2_callback, this, &PromptAuth::auto_close);

  auto action = findChild<QAction *>("action_close");
  addAction(action);
  connect(action, &QAction::triggered, this, &PromptAuth::reject,
          Qt::QueuedConnection);
}

PromptAuth::~PromptAuth() { delete ui; }

void PromptAuth::reject() {
  TheoreticalDiary::instance()->gwrapper->auth_err();
  /**
   * The window does not need to be closed here since
   * auth_err() actually emits sig_oauth2_complete
   * which is connected to auto_close() below
   * meaning, this window is closed anyway
   */
}

void PromptAuth::auto_close() { accept(); }
