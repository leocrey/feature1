#include "graphwindow.h"
#include "ui_graphwindow.h"

// Initialising the static variable
int GraphWindow::FigureCounter=0;


// constructor that is being called when XY graph is being plotted
GraphWindow::GraphWindow(DataSet *DataSet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphWindow)
{
    ui->setupUi(this);

    // Increment the figure counter:
    FigureCounter++;

    // Setting the paramters of the figure:
    SetFigureSetting();

    // Create the XY graph (setting te paramters of the dataset graph):
    SetGraphSetting(DataSet);

    // Setting the title of the figure
    this->setWindowTitle("Figure " + QString::number(FigureCounter));

    // Constructing the context menu so it is ready to be called whenever
    ConstructContextMenu(ContextMenu);

    // connect context menu actions signals and slots
    connect(actionEditStyle,SIGNAL(triggered()),this,SLOT(OpenGraphStyleDialog()));
    connect(actionAddData,SIGNAL(triggered()),this,SLOT(OpenPlotDataDialog()));

    // connect signals and slots for accessing datasets
    connect(this, SIGNAL(requestAllDataSets_SIGNAL()), parent, SLOT(receiveAllDataSetsRequest()));
    connect(parent, SIGNAL(sendAllDataSets_SIGNAL(QList<DataSet*>)), this, SLOT(receiveAllDataSets(QList<DataSet*>)));
}

// constructor that is called when histogram is being plotted
GraphWindow::GraphWindow(DataSet *DataSet, int numBins, QString histPlotName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphWindow)
{
    ui->setupUi(this);

    actionEditStyle->setEnabled(false);
    actionAddData->setEnabled(false);

    // Increment the figure counter:
    FigureCounter++;

    // Setting the paramters of the figure:
    SetFigureSetting();

    // turn dataset into a vector for histogram to interpret it
    QVector<double>* data = extractHistDataFromDataSet(DataSet);

    // Create the histogram (setting te paramters of the dataset graph):
    SetHistogramSetting(data, numBins, histPlotName);

    // Setting the title of the figure
    this->setWindowTitle("Figure " + QString::number(FigureCounter));

    actionEditStyle->setIcon(QIcon(":/icons/edit.svg"));
    actionAddData->setIcon(QIcon(":/icons/graph.svg"));

    // Constructing the context menu so it is ready to be called whenever
    ConstructContextMenu(ContextMenu);

    // connect context menu actions signals and slots
    connect(actionEditStyle,SIGNAL(triggered()),this,SLOT(OpenGraphStyleDialog()));
    connect(actionAddData,SIGNAL(triggered()),this,SLOT(OpenPlotDataDialog()));

    // connect signals and slots for accessing datasets
    connect(this, SIGNAL(requestAllDataSets_SIGNAL()), parent, SLOT(receiveAllDataSetsRequest()));
    connect(parent, SIGNAL(sendAllDataSets_SIGNAL(QList<DataSet*>)), this, SLOT(receiveAllDataSets(QList<DataSet*>)));
}

GraphWindow::~GraphWindow()
{ // Called when the window of the figure is closed
    delete actionEditStyle;
    delete actionAddData;
    delete ContextMenu;
    delete ui;
}

void GraphWindow::contextMenuEvent(QContextMenuEvent *event)
{ // This function is called when the user right-clicks on the graphwindow
    ContextMenu->popup(event->globalPos()); // displaying the menu where the user clicks
}

void GraphWindow::ConstructContextMenu(QMenu *)
{// This function is called in the constructor to build the context menu so that it does not need to be built everytime from scratch
    actionEditStyle->setIcon(QIcon(":/icons/edit.svg"));
    actionAddData->setIcon(QIcon(":/icons/graph.svg"));

    ContextMenu->addAction(actionEditStyle); // Add action
    ContextMenu->addAction(actionAddData); // Add sub menu
}

void GraphWindow::SetGraphSetting(DataSet *DataSet)
{ // Sets up the graph and plots it
    ui->customPlot->addGraph();
    int graphCount = ui->customPlot->graphCount();

    // graph(graphCount-1) is the graph that has just been added
    ui->customPlot->graph(graphCount-1)->addData(DataSet);
    ui->customPlot->graph(graphCount-1)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(graphCount-1)->setName(DataSet->getName());
    ui->customPlot->graph(graphCount-1)->rescaleAxes();
    ui->customPlot->replot();

    // add to a list of all graphs - for access in the style editing window
    AllGraphs.push_back(ui->customPlot->graph(graphCount-1));
}

void GraphWindow::SetHistogramSetting(QVector<double>* D, int numBins, QString histPlotName)
{
    // convert pointer to vector to real vector for histogram
    QVector<double> dataVector = *D;

    // creating empty histogram
    gsl_histogram * histogram = gsl_histogram_alloc (numBins);
    gsl_histogram_set_ranges_uniform(histogram, -1,1);

    // Populating the histogram with raw data
    for (int i = 0; i < dataVector.size(); i++)
        gsl_histogram_increment(histogram,dataVector[i]);

    // send data of each bin to data vector
    QVector<double> data;
    for (int i = 0; i < numBins; i++)
        data << gsl_histogram_get(histogram, i);

    // setup bars
    QCPBars *bar = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    bar->setAntialiased(false);
    bar->setPen(QPen(QColor(Qt::blue)));
    bar->setBrush(Qt::blue);

    QVector<double> ticks;
    QVector<QString> labels;
    for (int i = 1; i <= numBins; i++)
    {
        ticks << i;
        QString label = "Bin ";
        label += QString::number(i);
        labels << label;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->customPlot->xAxis->setTicker(textTicker);
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTicks(false);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->setRange(0, numBins + 1);
    ui->customPlot->xAxis->setBasePen(QPen(Qt::black));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::black));
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->customPlot->xAxis->setTickLabelColor(Qt::black);
    ui->customPlot->xAxis->setLabelColor(Qt::black);

    ui->customPlot->yAxis->setPadding(5); // padding on the left border
    ui->customPlot->yAxis2->setPadding(5); // padding on the right border
    ui->customPlot->yAxis->setLabel(histPlotName);
    ui->customPlot->yAxis->setBasePen(QPen(Qt::black));
    ui->customPlot->yAxis->setTickPen(QPen(Qt::black));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::black));
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->setTickLabelColor(Qt::black);
    ui->customPlot->yAxis->setLabelColor(Qt::black);
    ui->customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    ui->customPlot->setInteractions(QCP::iSelectPlottables); // dont want dragging and zooming on histogram
    ui->customPlot->legend->setVisible(false);

    int maxValue = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] > maxValue)
            maxValue = data[i];
    }
    ui->customPlot->yAxis->setRange(0, maxValue * 1.1);

    bar->setData(ticks, data);
}

void GraphWindow::SetFigureSetting()
{ // Sets up the properties of the figure (that contains the graph)

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void GraphWindow::OpenGraphStyleDialog()
{
    GraphStyleDialog* GraphStyle_dlg=new GraphStyleDialog(AllGraphs, this);
    GraphStyle_dlg->exec();
    delete GraphStyle_dlg;
}

void GraphWindow::receiveAllDataSets(QList<DataSet*> receivedDataSets)
{
    AllDataSets = receivedDataSets;
}

void GraphWindow::OpenPlotDataDialog()
{
    emit requestAllDataSets_SIGNAL();
    PlotDataDialog* PlotData_dlg=new PlotDataDialog(AllDataSets, this);
    PlotData_dlg->exec();
    delete PlotData_dlg;
}

void GraphWindow::receiveChosenDataSet(DataSet* chosenDataSet)
{

    SetGraphSetting(chosenDataSet);
}

void GraphWindow::ChangeGraphStyle(QCPGraph * graph, QPen * pen)
{
    graph->setPen(*pen);
    ui->customPlot->replot();
}

QVector<double>* GraphWindow::extractHistDataFromDataSet(DataSet* dataSet)
{
    QVector<double>* data = new QVector<double>;
    for (int i = 0; i < dataSet->Size(); i++)
    {
        data->push_back(dataSet->getPoint(i)[1]); // populate the data vector with the y-values of the dataset
    }

    return data;
}
