#include "filterselectionwindow.h"
#include "ui_filterselectionwindow.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QCheckBox>
FilterSelectionWindow::FilterSelectionWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterSelectionWindow)
{
    ui->setupUi(this);
    QPushButton * closeFiltersButton = new QPushButton("Cancel");
    this->closeFiltersButton = closeFiltersButton;
    QPushButton * applyFiltersButton = new QPushButton("Apply");
    this->applyFiltersButton = applyFiltersButton;

}

FilterSelectionWindow::~FilterSelectionWindow()
{
    delete ui;
}

QMap<QString, FilterSelectionWindow::selectorType> FilterSelectionWindow::getScalarSelectors()
{
    QMap<QString, FilterSelectionWindow::selectorType> selectors;
    const size_t layoutWidth = 4;
    const size_t layoutFromOffset = 2;
    const size_t layoutToOffset = 4;
    const double epsilon = 0.0001;
    for (size_t i = 0; i < filterNames.size(); ++i)
    {
        auto layout = dynamic_cast<QGridLayout *>(this->layout());
        auto lowerBoundWidget = layout->itemAtPosition(i, layoutFromOffset)->widget();
        auto upperBoundWidget = layout->itemAtPosition(i, layoutToOffset)->widget();
        double lowerBound = dynamic_cast<QLineEdit *>(lowerBoundWidget)->text().toDouble();
        double upperBound = dynamic_cast<QLineEdit *>(upperBoundWidget)->text().toDouble();

        selectors.insert(filterNames[i], [lowerBound, upperBound, this, i, epsilon](double attributeValue){
            if(this->nanCheckBoxes[i]->checkState() == Qt::Checked)
                return (attributeValue >= lowerBound - epsilon && attributeValue <= upperBound + epsilon) || std::isnan(attributeValue);
            return attributeValue >= lowerBound - epsilon && attributeValue <= upperBound + epsilon;
        });
    }
    return selectors;
}

void FilterSelectionWindow::clearForm()
{
    if(this->layout() == NULL)
        return;
    QLayoutItem * item;
    while((item = this->layout()->takeAt(0)) != NULL )
    {
        if(dynamic_cast<QPushButton*>(item->widget()) == nullptr)
        {
            delete item->widget();
            delete item;
        }
    }
    delete this->layout();
}
void FilterSelectionWindow::setFilterFields(const QVector<QString> & filters, const QVector<QString> & fromBaseValues,
                                            const QVector<QString> & toBaseValues)
{
    this->filterNames = filters;

    fromTextEdits.clear();
    toTextEdits.clear();
    nanCheckBoxes.clear();
    //QPushButton * closeFiltersButton = new QPushButton("Cancel");
    //this->closeFiltersButton = closeFiltersButton;
    //QPushButton * applyFiltersButton = new QPushButton("Apply");
    //this->applyFiltersButton = applyFiltersButton;

    clearForm();
    QGridLayout* layout = new QGridLayout(this);

    for (size_t i = 0; i < this->filterNames.size(); ++i)
    {
        auto filterName = this->filterNames[i];
        QLabel* filterLabel = new QLabel(filterName);
        QLabel* fromFilterLabel = new QLabel("from", this);
        QLabel* toFilterLabel = new QLabel("to", this);
        QLineEdit * fromTextEdit = new QLineEdit();
        fromTextEdit->setValidator(new QDoubleValidator());
        fromTextEdit->setMaximumSize(100, 30);
        fromTextEdit->setText(fromBaseValues[i]);
        fromTextEdits.push_back(fromTextEdit);

        QLineEdit * toTextEdit = new QLineEdit();
        toTextEdit->setValidator(new QDoubleValidator());
        toTextEdit->setMaximumSize(100, 30);
        toTextEdit->setText(toBaseValues[i]);
        toTextEdits.push_back(toTextEdit);

        QCheckBox * includeNanCheckBox = new QCheckBox("include NaN");
        layout->addWidget(filterLabel, i, 0);
        layout->addWidget(fromFilterLabel, i, 1);
        layout->addWidget(fromTextEdit, i, 2);
        layout->addWidget(toFilterLabel, i ,3);
        layout->addWidget(toTextEdit, i, 4);
        layout->addWidget(includeNanCheckBox, i, 5);
        nanCheckBoxes.push_back(includeNanCheckBox);


    }
    layout->addWidget(applyFiltersButton, this->filterNames.size(), 1);
    layout->addWidget(closeFiltersButton, this->filterNames.size(), 0);

    this->setLayout(layout);

}


