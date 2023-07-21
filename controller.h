#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QDebug>
#include "windowdatatablemodel.h"
#include <QAbstractTableModel>
#include <QItemSelection>
#include "filterselectionwindow.h"
QT_BEGIN_NAMESPACE
class MainWindow;
QT_END_NAMESPACE
//Controller from the MVC design pattern
//handles computations and signals from UI
class Controller : public QObject
{
    Q_OBJECT
    WindowDataTableModel* tableModel;
    MainWindow* mainWindow;

    FilterSelectionWindow * filterWindow;
    QString openFolderDialog();
    QString saveAsDialog();
    QString openFileDialog();
public:
    Controller(MainWindow *, FilterSelectionWindow *, WindowDataTableModel*) ;
public slots:
    void computeWindows(bool useRam = false);
    void normalizeCheckBoxChanged(bool checked);
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void saveSelectedWindows();
    void browseInputFile();
    void browseCalibFile();
    void openFiltersClicked();
    void closeFiltersClicked();
    void applyFiltersClicked();
    void view2DHistogramClicked();
    void selectComplementClicked();
    bool shouldPrintHeader(const QString & filename);
};

#endif // CONTROLLER_H
