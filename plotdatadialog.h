#ifndef PLOTDATADIALOG_H
#define PLOTDATADIALOG_H


/********************************
 *
 *  This class is defined to show the user a dialog to add a dataset to a pre-exisiting figure
 *  an object of this class is the window in which the user chooses what dataset to add to the figure
 *
 *
**********************************/


#include "dataset.h"
#include <QDialog>

namespace Ui {
class PlotDataDialog;
}

class PlotDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDataDialog(QList<DataSet*> AllDataSets, QWidget *parent = nullptr);
    ~PlotDataDialog();

private:
    Ui::PlotDataDialog *ui;
    QList<DataSet*> AllDataSets;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void sendChosenDataSet_SIGNAL(DataSet*);
};

#endif // PLOTDATADIALOG_H
