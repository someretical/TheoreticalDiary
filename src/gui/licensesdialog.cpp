#include "licensesdialog.h"
#include "../core/theoreticaldiary.h"
#include "ui_licensesdialog.h"

LicensesDialog::LicensesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LicensesDialog)
{
    ui->setupUi(this);

    QFile file(":/LICENSES.txt");
    file.open(QIODevice::ReadOnly);
    ui->licenses->setPlainText(file.readAll());
    file.close();

    connect(ui->ok_button, &QPushButton::clicked, this, &LicensesDialog::accept, Qt::QueuedConnection);

    connect(TheoreticalDiary::instance(), &TheoreticalDiary::apply_theme, this, &LicensesDialog::apply_theme,
        Qt::QueuedConnection);
    apply_theme();
}

LicensesDialog::~LicensesDialog()
{
    delete ui;
}

void LicensesDialog::apply_theme() {}
