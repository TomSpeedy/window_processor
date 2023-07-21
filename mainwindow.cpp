#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QObject>
#include <memory>
#include "filterselectionwindow.h"
#include "plot_drawing/qcustomplot/qcustomplot.h"
#include "QtConcurrent/QtConcurrent"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->chartView->setVisible(false);
    ui->loadedDataGroupBox->setVisible(false);
    filterWindow = new FilterSelectionWindow();
    dataTableModel = new WindowDataTableModel(ui->tableView);
    controller = new Controller(this, filterWindow, dataTableModel);
    ui->windowSizeTextBox->setText("200");
    ui->windowSizeTextBox->setValidator(new QDoubleValidator());
    ui->diffLineEdit->setValidator(new QDoubleValidator());
    QObject::connect(ui->computeWindowsButton, SIGNAL(clicked()), controller, SLOT(computeWindows()));
    QObject::connect(ui->normalizePropertiesCheckBox, SIGNAL(clicked(bool)), controller, SLOT(normalizeCheckBoxChanged(bool)));
    QObject::connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                     controller, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    QObject::connect(ui->browseInputButton, SIGNAL(clicked()), controller, SLOT(browseInputFile()));
    QObject::connect(ui->browseCalibButton, SIGNAL(clicked()), controller, SLOT(browseCalibFile()));
    QObject::connect(ui->saveSelectedButton, SIGNAL(clicked()), controller, SLOT(saveSelectedWindows()));
    QObject::connect(ui->selectByFiltersButton, SIGNAL(clicked()), controller, SLOT(openFiltersClicked()));
    QObject::connect(filterWindow->applyFiltersButton, SIGNAL(clicked()), controller, SLOT(applyFiltersClicked()));
    QObject::connect(filterWindow->closeFiltersButton, SIGNAL(clicked()), controller, SLOT(closeFiltersClicked()));
    QObject::connect(ui->view2DHistogramButton, SIGNAL(clicked()), controller, SLOT(view2DHistogramClicked()));
    QObject::connect(ui->selectComplementButton, SIGNAL(clicked()), controller, SLOT(selectComplementClicked()));
    filterWindow->hide();
    //ui->calibTextBox->setText("/home/tomas/MFF/DT/clusterer/test_data/calib/F4-W00076/");
    //ui->inputTextBox->setText("/home/tomas/MFF/DT/clusterer_data/pion/pions_180GeV_deg_0.txt");
    ui->chartView->setRubberBand(QChartView::HorizontalRubberBand);

}

void MainWindow::redraw3DPlots()
{
    if(dataTableModel->rowCount() < 1)
        return;
    const QString attributeToPlot = "e_distrib[[]]";
    const QString timeAttributeName = dataTableModel->columnNames()[0];
    auto targetColumnIndex = dataTableModel->columnNames().indexOf(attributeToPlot);
    using dataVector = std::vector<double>;
    size_t vectorFeatureSize = std::get<dataVector>(dataTableModel->getData()[0].get_value(attributeToPlot.toStdString())).size();
    QCustomPlot * histogram2DPlot = new QCustomPlot();
    histogram2DPlot->axisRect()->setupFullAxesBox(true);
    histogram2DPlot->xAxis->setLabel(timeAttributeName);
    histogram2DPlot->yAxis->ticker()->setTickCount(10);
    histogram2DPlot->yAxis->setLabel("bin_number");
    histogram2DPlot->yAxis->ticker()->setTickCount(vectorFeatureSize - 2);
    QCPColorMap * colorMap = new QCPColorMap(histogram2DPlot->xAxis, histogram2DPlot->yAxis);

    colorMap->data()->setSize(dataTableModel->rowCount(), vectorFeatureSize);
    colorMap->data()->setRange(
                QCPRange(0, std::get<double>(dataTableModel->getData()[dataTableModel->rowCount() - 1].get_value(timeAttributeName.toStdString()))),
                QCPRange(0, vectorFeatureSize));
    for(size_t i = 0; i < dataTableModel->rowCount(); i++)
    {
        double windowStartTime = std::get<double>(dataTableModel->getData()[i].get_value(timeAttributeName.toStdString()));
        dataVector vectorFeature =  std::get<dataVector>(dataTableModel->getData()[i].get_value(attributeToPlot.toStdString()));
        for (size_t j = 0; j < vectorFeature.size(); ++j)
        {
            colorMap->data()->setCell(i, j, vectorFeature[j]);
        }
    }
    QCPColorScale * colorScale = new QCPColorScale(histogram2DPlot);
    colorScale->setDataScaleType(QCPAxis::ScaleType::stLogarithmic);
    colorScale->axis()->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog()));
    colorMap->setColorScale(colorScale);
    colorMap->setGradient(QCPColorGradient::gpJet);
    colorMap->rescaleDataRange();
    colorMap->setInterpolate(false);

    histogram2DPlot->plotLayout()->addElement(0, 1, colorScale);
    histogram2DPlot->setGeometry(0,0, 1100, 500);
    histogram2DPlot->rescaleAxes();
    histogram2DPlot->setInteraction(QCP::iRangeZoom);
    histogram2DPlot->setInteraction(QCP::iRangeDrag);
    histogram2DPlot->replot();
    histogram2DPlot->show();

}

void MainWindow::redrawPlots()
{

    this->ui->tableView->setModel(this->dataTableModel);
    this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    QChart* chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    //QValueAxis* xAxis = new QValueAxis();
    //QLogValueAxis* yAxis = new QLogValueAxis();
    //xAxis->setTickCount(20);
    //xAxis->setLabelFormat("%g");
    //chart->addAxis(xAxis, Qt::AlignBottom);
    //chart->addAxis(yAxis, Qt::AlignLeft);
    auto selection = this->ui->tableView->selectionModel();
    if(dataTableModel->rowCount() == 0)
        return;

    //mark column selection
    for (int i = 1;  i < dataTableModel->columnCount(); i++)
    {
        if(selection->selectedColumns().size() == 0 || selection->isColumnSelected(i) ||
                (selection->isColumnSelected(0) && selection->selectedColumns().size() == 1))
        {

        QVXYModelMapper * bmMapper = new QVXYModelMapper();
        QString colName = dataTableModel->headerData(i, Qt::Horizontal).toString();
        QLineSeries * series = new QLineSeries();
        series->setName(colName);
        bmMapper->setXColumn(0);
        bmMapper->setYColumn(i);
        bmMapper->setSeries(series);
        bmMapper->setModel(dataTableModel);
        //series->attachAxis(xAxis);
        //series->attachAxis(yAxis);
        chart->addSeries(series);
        }
    }

    chart->createDefaultAxes();

    //chart->setTheme(QChart::ChartThemeDark);
    //mark row selection
    double rectWidth = (dynamic_cast<QValueAxis*>(chart->axisX())->max() - dynamic_cast<QValueAxis*>(chart->axisX())->min()) / (double)dataTableModel->rowCount();
    double rectHeight = dynamic_cast<QValueAxis*>(chart->axisY())->max() - dynamic_cast<QValueAxis*>(chart->axisY())->min();
    double yMin = dynamic_cast<QValueAxis*>(chart->axisY())->min();
    QAreaSeries * rowSelectSeries;
    QLineSeries * seriesRect1 = new QLineSeries();
    QLineSeries * seriesRect2 = new QLineSeries();
    for (int i = 0;  i < dataTableModel->rowCount(); i++)
    {
        if(selection->isRowSelected(i))
        {
            if(i == 0 || !selection->isRowSelected(i-1))
            {

                seriesRect1 = new QLineSeries();
                seriesRect2 = new QLineSeries();
            }
            *seriesRect1 << QPointF(i * rectWidth, yMin) << QPointF((i + 1) * rectWidth, yMin);
            *seriesRect2 << QPointF(i * rectWidth, yMin + rectHeight) << QPointF((i + 1) * rectWidth, yMin + rectHeight);

            if(i == dataTableModel->rowCount() || !selection->isRowSelected(i+1))
            {
                rowSelectSeries = new QAreaSeries(seriesRect2, seriesRect1);


                rowSelectSeries->setColor(QColor(255,0,0,50));
                rowSelectSeries->setBorderColor(Qt::transparent);
                chart->addSeries(rowSelectSeries);
                chart->legend()->markers(rowSelectSeries)[0]->setVisible(false);
            }

        }
    }
    //TODO problem - creates separate axis ranges for each series
    chart->createDefaultAxes();
    chart->axes().front()->setTitleText(dataTableModel->columnNames()[0]);
    this->ui->chartView->setChart(chart);
    this->ui->chartView->setRenderHint(QPainter::Antialiasing);
    this->ui->chartView->setVisible(true);

}

void MainWindow::setVectorComboBox()
{
    ui->vectorPropertyComboBox->clear();
    for (auto && attributeName : dataTableModel->columnNames())
    {
        if(dataTableModel->isVectorProperty(attributeName))
            ui->vectorPropertyComboBox->addItem(attributeName);
    }


}

void MainWindow::displayLoadingGif()
{
    QMovie* loader = new QMovie("/home/tomas/MFF/DT/window_processor/images/loading.gif");
    loader->setScaledSize(QSize(100, 100));
    this->ui->labelLoading->setEnabled(true);
    this->ui->labelLoading->show();
    this->ui->labelLoading->setMovie(loader);
    this->ui->labelLoading->setGeometry(this->ui->labelLoading->x(), this->ui->labelLoading->y(), 100, 100);
    //QtConcurrent::run([loader](){loader->start();});
    loader->start();
}
void MainWindow::hideLoadingGif()
{
    this->ui->labelLoading->hide();
    //this->ui->labelLoading->setMovie(nullptr);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete controller;
    delete filterWindow;
}



