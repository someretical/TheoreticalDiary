#include "aboutdialog.h"
#include "../core/theoreticaldiary.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  ui->version_placeholder->setText(QApplication::applicationVersion());

  QFile file(":/CONTRIBUTORS.txt");
  file.open(QIODevice::ReadOnly);
  ui->contributors->setPlainText(file.readAll());
  file.close();

  connect(ui->ok_button, &QPushButton::clicked, this, &AboutDialog::accept,
          Qt::QueuedConnection);

  connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this,
          &AboutDialog::apply_theme, Qt::QueuedConnection);
  apply_theme();
}

AboutDialog::~AboutDialog() { delete ui; }

void AboutDialog::apply_theme() {
  QFile file(":/global/aboutdialog.qss");
  file.open(QIODevice::ReadOnly);
  setStyleSheet(file.readAll());
  file.close();
}
