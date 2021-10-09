#include "promptauth.h"
#include "googlewrapper.h"
#include "theoreticaldiary.h"
#include "ui_promptauth.h"

PromptAuth::PromptAuth(QWidget *parent)
    : QDialog(parent), ui(new Ui::PromptAuth) {
  ui->setupUi(this);

  connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_auth_ok,
          this, &PromptAuth::auto_close);
  connect(TheoreticalDiary::instance()->gwrapper, &GoogleWrapper::sig_auth_err,
          this, &PromptAuth::auto_close);

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
   * auth_err() actually emits sig_auth_err
   * which is connected to auto_close() below
   * meaning, this window is closed anyway
   */
}

void PromptAuth::auto_close() { accept(); }
