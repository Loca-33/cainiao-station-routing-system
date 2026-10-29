#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>

namespace Ui {
class ResultDialog;
}

class ResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultDialog(const QString &title, const QString &content, QWidget *parent = nullptr);
    ~ResultDialog();

private:
    Ui::ResultDialog *ui;
};

#endif // RESULTDIALOG_H
