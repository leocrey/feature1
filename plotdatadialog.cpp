#include "plotdatadialog.h"
#include "ui_plotdatadialog.h"

PlotDataDialog::PlotDataDialog(QList<DataSet*> dataSetList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDataDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Add Data To Plot");

    AllDataSets = dataSetList;

    foreach(DataSet* dataSet, dataSetList)
    {
        ui->dataSelectionBox->addItem(dataSet->getName());
    }

    connect(this, SIGNAL(sendChosenDataSet_SIGNAL(DataSet*)), parent, SLOT(receiveChosenDataSet(DataSet*)));
}

PlotDataDialog::~PlotDataDialog()
{
    delete ui;
}

void PlotDataDialog::on_buttonBox_accepted()
{
    int index = ui->dataSelectionBox->currentIndex();
    DataSet* selectedDataSet = AllDataSets[index];

    emit sendChosenDataSet_SIGNAL(selectedDataSet);

    PlotDataDialog::close();
}


void PlotDataDialog::on_buttonBox_rejected()
{
    PlotDataDialog::close();
}

