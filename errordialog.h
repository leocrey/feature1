#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorDialog;
}

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorDialog(int errCode, QWidget *parent = nullptr);
    ~ErrorDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ErrorDialog *ui;
    QString getErrorDetails(int errCode);
};

#endif // ERRORDIALOG_H
