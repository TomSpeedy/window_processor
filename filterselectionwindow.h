#ifndef FILTERSELECTIONWINDOW_H
#define FILTERSELECTIONWINDOW_H

#include <QWidget>
#include "ui_filterselectionwindow.h"
#include <QPushButton>
#include <functional>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
//A user interface for selecting filters for window selection
namespace Ui {
class FilterSelectionWindow;
}

class FilterSelectionWindow : public QWidget
{
    Q_OBJECT
    QVector<QString> filterNames;

public:
    //Child widgets
    QPushButton * applyFiltersButton;
    QPushButton * closeFiltersButton;
    QVector<QLineEdit*> fromTextEdits;
    QVector<QLineEdit*> toTextEdits;
    QVector<QCheckBox*> nanCheckBoxes;
    explicit FilterSelectionWindow(QWidget *parent = nullptr);
    using selectorType =  std::function<bool(double)>;

    ~FilterSelectionWindow();
    void setFilterFields(const QVector<QString> & filters, const QVector<QString> & fromBaseValues,
                                        const QVector<QString> & toBaseValues);
    void display();
    QMap<QString, selectorType> getScalarSelectors();
    void clearForm();
    Ui::FilterSelectionWindow *ui;
};

#endif // FILTERSELECTIONWINDOW_H
