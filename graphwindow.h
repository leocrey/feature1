#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

/********************************
 *
 *  This class is defined to show a graph in a window,
 *  an object of this class is the window in which a graph is plotted
 *
 *  In this class, a figure is the frame of the plot of the curves, its
 *  properties include the axes, the grid, the title, the legend.
 *
 *  In this class, a graph is the curve plotted, its properties include
 *  the width of the line, its style, and its colour.
 *
 *
**********************************/

#include <QDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QVector>
#include "gsl/gsl_histogram.h"
#include "dataset.h"
#include "qcustomplot.h"
#include "graphstyledialog.h"
#include "plotdatadialog.h"

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GraphWindow(DataSet *DataSet, QWidget *parent = nullptr);
    explicit GraphWindow(DataSet *DataSet, int numBins, QString histPlotName, QWidget *parent = nullptr);
    ~GraphWindow();



    void SetGraphSetting(DataSet *DataSet); // Function to control the setting of the curve (dataset represenation in the figure)
    void SetHistogramSetting(QVector<double>* data, int numBins, QString histPlotName);
    QVector<double>* extractHistDataFromDataSet(DataSet* dataSet);
    void SetFigureSetting(); // Function to control the setting of the figure itself

    static int FigureCounter; // Number of Figures created in the app ( defined as static because it is shared among all objects of this class)
    QList<QCPGraph*> AllGraphs;

private:
    Ui::GraphWindow *ui;
    QList<DataSet*> AllDataSets;

    // context menu
    QMenu *ContextMenu=new QMenu(this);

    // sub-menu and actions for adding data
    QAction* actionAddData = new QAction("Add Data", this);

    // other actions
    QAction* actionEditStyle=new QAction("Edit Style", this);

    void contextMenuEvent(QContextMenuEvent *event);
    void ConstructContextMenu(QMenu *);

private slots:
    void ChangeGraphStyle(QCPGraph * graph, QPen* pen);
    void OpenGraphStyleDialog();
    void OpenPlotDataDialog();
    void receiveAllDataSets(QList<DataSet*> receivedDataSets);
    void receiveChosenDataSet(DataSet* chosenDataSet);

signals:
    void requestAllDataSets_SIGNAL();
};

#endif // GRAPHWINDOW_H
