#include "./mainwindow.h"
#include "controller.h"
#include "./ui_mainwindow.h"
#include "filterselectionwindow.h"
#include <QtDataVisualization>
//#include "../clusterer/include/data_flow/dataflow_controller.h"
//#include "../clusterer/include/execution/executor.h"
//#include "../clusterer/include/execution/model_runner.h"
#include "execution/executor.h"
#include "execution/model_runner.h"
#include "data_flow/dataflow_controller.h"
//expects to obtain UI that is fully setup - containing tableView
Controller::Controller(MainWindow * mainWindow, FilterSelectionWindow * filterWindow, WindowDataTableModel* tableModel) :
    tableModel(tableModel),
    mainWindow(mainWindow),
    filterWindow(filterWindow)
{
    mainWindow->redrawPlots();
}

void Controller::openFiltersClicked()
{

    filterWindow->show();
}
void Controller::closeFiltersClicked()
{
   filterWindow->hide();
}

void Controller::view2DHistogramClicked()
{
    mainWindow->redraw3DPlots();
}
void informMessageBox(const QString & text)
{
    QMessageBox msg_box;
    msg_box.setText(text);
    msg_box.exec();
}

void Controller::applyFiltersClicked()
{
    auto selectors = filterWindow->getScalarSelectors();
    QVector<size_t> selectedIndices = tableModel->getRowsBySelectors(selectors);
    auto selection  = mainWindow->ui->tableView->selectionModel()->selection();
    for (auto && selectedIndex : selectedIndices)
    {
        selection.merge(QItemSelection(tableModel->index(selectedIndex, 0), tableModel->index(selectedIndex, tableModel->columnCount() - 1)),
                        QItemSelectionModel::Select);
        std::cout << "Selected row " << selectedIndex << std::endl;
    }
    mainWindow->ui->tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
    mainWindow->redrawPlots();
    filterWindow->hide();
    informMessageBox("Successfully selected " + QString::number(selectedIndices.size()) + " rows");
}
void Controller::normalizeCheckBoxChanged(bool checked)
{
    if(checked)
        tableModel->normalize();
    else
        tableModel->unnormalize();
    mainWindow->redrawPlots();
}

template <typename inputType, typename outputType>
QVector<outputType> transformVector(const QVector<inputType> & inputVector, std::function<outputType(inputType)> operation)
{
    QVector<outputType> result;
    std::transform(inputVector.begin(), inputVector.end(), std::back_inserter(result), operation);
    return result;
}
void Controller::computeWindows(bool useRam)
{
    std::vector<std::string> args;
    QString temp_output = "./temp/current_temp";
    args = {"/home/tomas/MFF/DT/fieldData/SPS_beam/pions_180GeV_bias200V_120deg_angle_scan_after_widening_800s_F4-W00076.txt", "/home/tomas/MFF/DT/clusterer/test_data/calib/", "", "4"};

        std::string data_file = mainWindow->ui->inputTextBox->toPlainText().toStdString();//args[0];
        std::string calib_folder = mainWindow->ui->calibTextBox->toPlainText().toStdString(); //args[1];
        std::string arch_type = args[2];
        uint16_t clustering_cores = std::stoi(args[3]);
        if(!std::filesystem::exists("temp"))
            std::filesystem::create_directory("temp");
        //benchmarker* bench = new benchmarker(data_file, calib_folder, temp_output.toStdString());
        //
        model_executor * executor = new model_executor(std::vector<std::string> {data_file}, std::vector<std::string>{calib_folder}, temp_output.toStdString());
        model_runner::recurring = false;
        model_runner::print = true;
        node_args n_args;
        n_args["window_computer"]["window_size"] = std::to_string(mainWindow->ui->windowSizeTextBox->text().toDouble() * 1000000);
        n_args["window_computer"]["diff_window_size"] = std::to_string(mainWindow->ui->diffLineEdit->text().toDouble() * 1000000);
        model_runner::run_model(model_runner::model_name::WINDOW_COMPUTER, executor, 1, n_args);
        //delete tableModel;
        mainWindow->ui->tableView->reset();
        tableModel->set(&temp_output);
        auto colNames = tableModel->columnNames();

        filterWindow->setFilterFields(colNames.mid(0, colNames.size() - 1),
                                      transformVector<double, QString>(tableModel ->getMinValues(),
                                                                       [](double value){ return QString::number(value);}),
                                      transformVector<double, QString>(tableModel ->getMaxValues(),
                                                                       [](double value){ return QString::number(value);}));
        mainWindow->redrawPlots();
        mainWindow->setVectorComboBox();
        mainWindow->ui->loadedDataGroupBox->setVisible(true);
        informMessageBox("Window features loaded successfully");
        //delete bench;
        delete executor;
}
QString generateTimestamp()
{
    auto dateTime = QDateTime::currentDateTime();
    return dateTime.toString().replace(":", "-");
}
bool Controller::shouldPrintHeader(const QString & filename)
{
    //only print header if we start a new file or if the file is empty
    return (!QFile::exists(filename) || QFile(filename).size() == 0);

}
void Controller::saveSelectedWindows()
{
    const QString windowSuffix = ".wf";
    if(!QDir("output").exists())
        QDir().mkdir("output");
    QString filename = saveAsDialog();//"./output/saved_window_features_" + generateTimestamp();
    if(!filename.endsWith(windowSuffix))
        filename += windowSuffix;
    std::ofstream selectedWindowsStream(filename.toStdString(), std::ios_base::app);
    if(!selectedWindowsStream.is_open())
    {
        throw std::invalid_argument("Could not write selected windows to a file");
    }
    auto selection = mainWindow->ui->tableView->selectionModel();
    if(shouldPrintHeader(filename))
    {
        selectedWindowsStream << "#Attribute_count:" << tableModel->columnCount() << std::endl;
        for(size_t i = 0; i < tableModel->columnCount(); ++i)
        {
            selectedWindowsStream << "#Attribute_" << i << ":" << tableModel->columnNames()[i].toStdString() << std::endl;
        }
    }
    selectedWindowsStream << "#Class:" << mainWindow->ui->fieldNameTextBox->toPlainText().toStdString() << std::endl;
    selectedWindowsStream << "#Count:" << selection->selectedRows().size() << std::endl;

    for(size_t i = 0; i < tableModel->rowCount(); ++i)
    {
       if(selection->isRowSelected(i))
       {
           tableModel->printRow(i, selectedWindowsStream);
       }
    }
    selectedWindowsStream.close();
    informMessageBox("Selected window features were saved to " + filename);


}
QString Controller::saveAsDialog()
{
    return QFileDialog::getSaveFileName(mainWindow, tr("Save File"), ".", tr("window features(*.wf)"));
}
QString Controller::openFolderDialog()
{
    return QFileDialog::getExistingDirectory(mainWindow, tr("Open Folder"));


}

QString Controller::openFileDialog()
{
    return QFileDialog::getOpenFileName(mainWindow, tr("Open File"));
}

void Controller::browseInputFile()
{
    auto dialogResult = openFileDialog();
    if(dialogResult != "")
        mainWindow->ui->inputTextBox->setText(dialogResult);

}

void Controller::browseCalibFile()
{
    auto dialogResult = openFolderDialog();
    if(dialogResult != "")
        mainWindow->ui->calibTextBox->setText(dialogResult);
}

void Controller::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    mainWindow->redrawPlots();
}

void Controller::selectComplementClicked()
{
    auto selectionModel = mainWindow->ui->tableView->selectionModel();
    auto selection = mainWindow->ui->tableView->selectionModel()->selection();
    auto selectedColumns = selectionModel->selectedColumns();
    auto selected = QItemSelection();
    auto deselected = QItemSelection();
    for (int rowIndex = 0; rowIndex < tableModel->rowCount(); ++rowIndex)
    {
        if(! selectionModel->isRowSelected(rowIndex))
            selected.merge(QItemSelection(tableModel->index(rowIndex, 0),
                                                  tableModel->index(rowIndex, tableModel->columnCount() - 1)), QItemSelectionModel::Select);
        else
            deselected.merge(QItemSelection(tableModel->index(rowIndex, 0),
                                                  tableModel->index(rowIndex, tableModel->columnCount() - 1)), QItemSelectionModel::Deselect);

    }
    selectionModel->select(selection, QItemSelectionModel::Deselect);
    selectionModel->select(selected, QItemSelectionModel::Select);
    //selectionModel->select(deselected, QItemSelectionModel::Deselect);
    for(int colIndex = 0; colIndex < selectedColumns.size(); ++colIndex)
    {
        //selectionModel->select(selectedColumns[colIndex], QItemSelectionModel::Select);
    }
    mainWindow->redrawPlots();
}
