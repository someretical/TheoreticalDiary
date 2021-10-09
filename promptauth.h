#ifndef PROMPTAUTH_H
#define PROMPTAUTH_H

#include <QDialog>

namespace Ui {
class PromptAuth;
}

class PromptAuth : public QDialog {
  Q_OBJECT

public:
  explicit PromptAuth(QWidget *parent = nullptr);
  ~PromptAuth();

public slots:
  void reject();
  void auto_close();

private:
  Ui::PromptAuth *ui;
};

#endif // PROMPTAUTH_H
