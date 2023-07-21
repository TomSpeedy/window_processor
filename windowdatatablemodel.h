#ifndef WINDOWDATATABLEMODEL_H
#define WINDOWDATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <iostream>
#include <fstream>
#include "data_nodes/window_processing/default_window_feature_vector.h"
#include "data_structs/mm_hit.h"

//This class implements the Model from MVC design pattern
//it stores the loaded data and provides access to further computations
class WindowDataTableModel : public QAbstractTableModel
{

public:
    using FeatureVectType = default_window_feature_vector<mm_hit>;
private:
    //basic properties of the loaded data
    bool loaded_ = false;
    bool normalizeData = false;
    QVector<double> minValues;
    QVector<double> maxValues;
    QVector<QVariant> colNames;
    //variable storing the loaded data
    QVector<FeatureVectType> data_;
    //the selector used for window filtering
    using selectorType = std::function<bool(double)>;
public:
    //columnwise normalization of the features
    void normalize();
    void unnormalize();
    const QVector<FeatureVectType> & getData() const;
    explicit WindowDataTableModel(QObject *parent = nullptr);
    void set(QString * inputFilename);
    //4 methods required for correct displaying of the table
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //outut a single row of table to file
    void printRow(int index, std::ofstream & output) const;
    QVector<QString> columnNames();
    const QVector<double> & getMinValues() const;
    const QVector<double> & getMaxValues() const;
    //returns indices of rows for which all selectors returned true
    QVector<size_t> getRowsBySelectors(QMap<QString, selectorType> selectors);
    //checks if property can be used for filtering
    bool isVectorProperty(const QString & propertyName) const;
};

#endif // WINDOWDATATABLEMODEL_H
