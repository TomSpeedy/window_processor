#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "controller.h"
#include "windowdatatablemodel.h"
#include "ui_mainwindow.h"
#include "filterselectionwindow.h"
#pragma once

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void redrawPlots();
    void redraw3DPlots();
    void setVectorComboBox();

friend class Controller;
private:
    Ui::MainWindow *ui;

    Controller* controller;
    WindowDataTableModel* dataTableModel;
    FilterSelectionWindow * filterWindow;
};
#endif // MAINWINDOW_H
