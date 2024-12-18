#include "changedatasetinfodialog.h"
#include "ui_changedatasetinfodialog.h"

ChangeDataSetInfoDialog::ChangeDataSetInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeDataSetInfoDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Change Dataset Information");
    this->setWindowIcon(QIcon(":/icons/edit.svg"));

    // Connect both name and description signals
    connect(this, SIGNAL(sendNewDataSetName_SIGNAL(QString*)), parent,
            SLOT(receiveNewDataSetName(QString*)));
    connect(this, SIGNAL(sendNewDataSetDescription_SIGNAL(QString*)), parent,
            SLOT(receiveNewDataSetDescription(QString*)));
}

ChangeDataSetInfoDialog::~ChangeDataSetInfoDialog()
{
    delete ui;
}

void ChangeDataSetInfoDialog::on_buttonBox_accepted()
{
    try {
        // Handle name input
        QString* input = new QString(ui->lineEdit->text());
        int inputLength = input->length();
        if(inputLength > 0)
        {
            emit sendNewDataSetName_SIGNAL(input);

            // Handle description input
            QString* description = new QString(ui->textEdit->toPlainText());  // Assuming your description TextEdit is named "textEdit"
            emit sendNewDataSetDescription_SIGNAL(description);
        } else throw 2;
    }
    catch(int errCode)
    {
        displayErrorDialog(errCode);
    }
    ChangeDataSetInfoDialog::close();
}

void ChangeDataSetInfoDialog::on_buttonBox_rejected()
{
    ChangeDataSetInfoDialog::close();
}

void ChangeDataSetInfoDialog::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}

void ChangeDataSetInfoDialog::setExistingDescription(const QString& desc)
{
    ui->textEdit->setPlainText(desc);  // Assuming your QTextEdit is named "textEdit"
}
