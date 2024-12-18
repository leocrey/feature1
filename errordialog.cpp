#include "errordialog.h"
#include "ui_errordialog.h"

ErrorDialog::ErrorDialog(int errCode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icons/errorSymbol.svg"));
    this->setWindowTitle("Error");

    QString errNum = "ERROR CODE: " + QString::number(errCode);
    QString errDetails = getErrorDetails(errCode);
    ui->errNumLabel->setText(errNum);
    ui->errDetailsText->setPlainText(errDetails);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::on_pushButton_clicked()
{
    ErrorDialog::close();
}

QString ErrorDialog::getErrorDetails(int errCode)
{
    switch(errCode){
        case 1:
            return "You are attempting to plot a dataset that is not loaded. Load the dataset into the application to be able to plot it.";
        case 2:
            return "Dataset name must be 1 character or longer.\n\nDataset has not been renamed. Please try again.";
        case 3:
            return "Error occured while setting line style. Default option 'Solid' has been used.";
        case 4:
            return "Error occured while setting line width. Default option '1' has been used.";
        case 5:
            return "A name must be entered for the histogram plot.\n\nHistogram has not been created. Please try again.";
        case 6:
            return "A value (1-100) must be entered for the number of bins.\n\nHistogram has not been created. Please try again.";
        case 7:
            return "Function analysis requires use of 2 seperate datasets. Make sure you have entered the correct dataset names\n\nOperation has been cancelled. Please try again.";
        default:
            return "Valid error code not received.";
    }
}

