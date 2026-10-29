#include "ResultDialog.h"
#include "ui_ResultDialog.h"

ResultDialog::ResultDialog(const QString &title, const QString &content, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResultDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->resultTextEdit->setPlainText(content);
}

ResultDialog::~ResultDialog()
{
    delete ui;
}
