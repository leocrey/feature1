#include "parentwindow.h"
#include "ui_parentwindow.h"

ParentWindow::ParentWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ParentWindow)
{ // This block is called when the app is started
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/DataViz.png"));
    this->setWindowTitle("DataViz 0.2");

    // Setting the Main widget of the application
    setCentralWidget(ui->WindowsManager);

    // configure main menu actions
    ui->menuXY_Plot->setIcon(QIcon(":/icons/graph.svg"));
    ui->actionHist_Plot->setIcon(QIcon(":/icons/histogram.svg"));
    ui->menuXY_Plot->setEnabled(false);
    ui->actionHist_Plot->setEnabled(false);
    ui->actionFunction->setEnabled(false);

    // connect signal and slot for plotting XY graph from main menu
    connect(this, SIGNAL(PlotXYData_SIGNAL(DataSet*)), this, SLOT(GraphWindowToBePlotted(DataSet*)));
}

ParentWindow::~ParentWindow()
{ // This block is called when the app is closing (to clean up all memory allocation used when the app started)
    delete actionXY_Plot;
    delete ui;
}

void ParentWindow::receiveAllDataSetsRequest()
{
    emit sendAllDataSets_SIGNAL(AllDataSets);
}

void ParentWindow::on_actionLoad_Dataset_triggered()
{ // This block is called when the user triggers Load action to load a file


    // Opening a file dialog
    QString curPath=QDir::currentPath(); // Directs the "open file" to the current directory
    QString FileName=QFileDialog::getOpenFileName(this,tr("Open file"),curPath,tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;All files(*.*)"));

    if (FileName.isEmpty())
        return; //If no file is selected don't do anything further

    OpenDataSet(&FileName);

}

void ParentWindow::OpenDataSet(QString* FileName)
{// called when loading dataset or a new dataset has been created using 'Function' action

    // Creating a new dataset in the app
    AddedDataSet= new DataSet(*FileName);
    if (AddedDataSet->IsDataSetValid) // Making sure the dataset is only dealt with if it was loaded succsessfully
    {

        AllDataSets.push_back(AddedDataSet); // Addding a pointer to the new dataset so that it can be accessed by the rest of the app


        // Main menu actions for plotting new dataset
        actionXY_Plot = new QAction("Dataset \""+AddedDataSet->getName()+"\"", this);
        ui->menuXY_Plot->addAction(actionXY_Plot);
        connect(actionXY_Plot, SIGNAL(triggered()), this, SLOT(PlotXYData()));

        // Create a subWindow for the loaded DataSet
        AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
        subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
        AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)


        // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
        // of an already displayed DataSetWindow
        connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
        connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

        // To remove a dataset when the dataset window is closed
        connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

        // Refresh the actions under 'Plot -> XY Plot' when dataset is renamed or deleted
        connect(AddedDataSetWindow, SIGNAL(refreshPlottingActions_SIGNAL()), this, SLOT(refreshPlottingActions()));

        // now data set has been loaded, can enable the menu actions that require a dataset
        ui->menuXY_Plot->setEnabled(true);
        ui->actionHist_Plot->setEnabled(true);
        ui->actionFunction->setEnabled(true);
    }
}

void ParentWindow::PlotXYData()
{
    QAction* senderAction = qobject_cast<QAction*>(sender());

    // compare each action in the menu to the action that sent the signal.
    // if they match, the index of this action will match the index of the chosen dataset
    int i = 0;
    foreach (QAction *tempAction, ui->menuXY_Plot->actions())
    {
        if (tempAction == senderAction)
        {
            emit PlotXYData_SIGNAL(AllDataSets[i]);
            break;
        } else {
            i++;
        }

        // if end of action list is reached and none were a match, throw an error
        if (i == ui->menuXY_Plot->actions().length())
        {
            int errCode = 1;
            displayErrorDialog(errCode);
        }
    }
}

void ParentWindow::GraphWindowToBePlotted(DataSet *ptr)
{ // This function is a slot that is called when the parentwindow is to plot a dataset in from a datasetwindow
    GraphWindow *AddedGraphWindow=new GraphWindow(ptr, this);
    subWindow=ui->WindowsManager->addSubWindow(AddedGraphWindow);
    AddedGraphWindow->show();
}

void ParentWindow::on_actionAbout_triggered()
{ // This function is called when the user clicks on "About" option under "About" menu
    AboutDialog* About_dlg=new AboutDialog(this);
    About_dlg->exec();
    delete About_dlg;
}

void ParentWindow::on_actionHelp_triggered()
{// This function is called when the user clicks on "Help" option under "Help" menu
    HelpDialog* Help_dlg=new HelpDialog(this);
    Help_dlg->exec();
    delete Help_dlg;
}

void ParentWindow::on_actionHist_Plot_triggered()
{// This function is called when the user selects to plot a histogram
    PlotHistogramDataDialog* PlotHist_dlg=new PlotHistogramDataDialog(AllDataSets, this);
    PlotHist_dlg->exec();
    delete PlotHist_dlg;
}

void ParentWindow::on_actionFunction_triggered()
{// This function is called when the user clicks on "Function" option under "Analysis" menu
    FunctionDialog* Function_dlg=new FunctionDialog(AllDataSets,this);
    Function_dlg->exec();

    // Create a new dataset from the computed expression
    DataSet *AddedDataSet = new DataSet(Function_dlg->getResult());


    AllDataSets.push_back(AddedDataSet); // Addding a pointer to the new dataset so that it can be accessed by the rest of the app

    // Main menu actions for plotting new dataset
    actionXY_Plot = new QAction("Dataset \""+AddedDataSet->getName()+"\"", this);
    ui->menuXY_Plot->addAction(actionXY_Plot);
    connect(actionXY_Plot, SIGNAL(triggered()), this, SLOT(PlotXYData()));

    // Create a subWindow for the loaded DataSet
    AddedDataSetWindow=new DataSetWindow(AddedDataSet,this);
    subWindow=ui->WindowsManager->addSubWindow(AddedDataSetWindow);
    AddedDataSetWindow->show(); // showing the new dataset window to the user (when it is added for the first time)


    // To enable the ParentWindow to plot the dataset when the user clicks on a plot option in the context menu
    // of an already displayed DataSetWindow
    connect(AddedDataSetWindow,SIGNAL(Plot_XYPlot_SIGNAL(DataSet*)),this,SLOT(GraphWindowToBePlotted(DataSet*)));
    connect(AddedDataSetWindow,SIGNAL(Open_HistPlotDialog_SIGNAL(DataSet*)),this,SLOT(OpenHistPlotDialog(DataSet*)));

    // To remove a dataset when the dataset window is closed
    connect(AddedDataSetWindow, SIGNAL(deleteDataSet_SIGNAL(DataSet*)), this, SLOT(deleteDataSet(DataSet*)));

    // Refresh the actions under 'Plot -> XY Plot' when dataset is renamed or deleted
    connect(AddedDataSetWindow, SIGNAL(refreshPlottingActions_SIGNAL()), this, SLOT(refreshPlottingActions()));

    // now data set has been loaded, can enable the menu actions that require a dataset
    ui->menuXY_Plot->setEnabled(true);
    ui->actionHist_Plot->setEnabled(true);

    delete AddedDataSet;

    delete Function_dlg;
}

void ParentWindow::OpenHistPlotDialog(DataSet* selectedDataSet)
{// Called when Histogram dialog box is opened from context menu. Passes over the index of the dataset that was right-clicked
    int dataSetIndex = 0;

    for (int i = 0; i < AllDataSets.length(); i++)
    {
        if (selectedDataSet == AllDataSets[i])
        {
            dataSetIndex = i;
            break;
        }
    }

    PlotHistogramDataDialog* PlotHist_dlg=new PlotHistogramDataDialog(AllDataSets, dataSetIndex, this);
    PlotHist_dlg->exec();
    delete PlotHist_dlg;
}

void ParentWindow::displayErrorDialog(int errCode)
{
    ErrorDialog* Error_dlg=new ErrorDialog(errCode, this);
    Error_dlg->exec();
    delete Error_dlg;
}

void ParentWindow::deleteDataSet(DataSet* dataSet)
{
    // cycle through all data sets to remove the deleted one
    for (int i = 0; i < AllDataSets.length(); i++)
    {
        if (AllDataSets[i] == dataSet)
        {
            AllDataSets.removeAt(i);
        }
    }

    // disable menu actions if no datasets are left open
    if (AllDataSets.length() == 0)
    {
        AddedDataSet = nullptr;
        ui->menuXY_Plot->setEnabled(false);
        ui->actionHist_Plot->setEnabled(false);
        ui->actionFunction->setEnabled(false);
    }

    refreshPlottingActions();
}

void ParentWindow::refreshPlottingActions()
{
    // remove all actions
    foreach (QAction* action, ui->menuXY_Plot->actions())
    {
        ui->menuXY_Plot->removeAction(action);
    }

    // add back the actions for datasets that remain
    foreach (DataSet* dataSet, AllDataSets)
    {
        // XY plot actions
        actionXY_Plot = new QAction("Dataset \""+dataSet->getName()+"\"", this);
        ui->menuXY_Plot->addAction(actionXY_Plot);
        connect(actionXY_Plot, SIGNAL(triggered()), this, SLOT(PlotXYData()));
    }
}

void ParentWindow::receiveHistogramData(DataSet* dataSet, int numBins, QString histPlotName)
{// when histogram data is receive, open a graph window to plot histogram
    GraphWindow *AddedGraphWindow=new GraphWindow(dataSet, numBins, histPlotName, this);
    subWindow=ui->WindowsManager->addSubWindow(AddedGraphWindow);
    AddedGraphWindow->show();
}


void ParentWindow::SaveFunctionDataToFile(QVector<double>* dataVector)
{
   // TODO in a future version
}

