#ifndef WINDOWDATATABLEMODEL_H
#define WINDOWDATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <iostream>
#include <fstream>
//#include "include/data_nodes/window_processing/default_window_feature_vector.h"
//#include "default_window_feature_vector.h"

#include "data_nodes/window_processing/default_window_feature_vector.h"
#include "data_structs/mm_hit.h"

class WindowDataTableModel : public QAbstractTableModel
{

public:
    using FeatureVectType = default_window_feature_vector<mm_hit>;
private:
    bool loaded_ = false;
    int rowCountVal = 10;
    int columnCountVal = 3;
    bool normalizeData = false;
    QVector<double> minValues;
    QVector<double> maxValues;
    QVector<QVariant> colNames = {QVariant(tr("hello1")),
                                  QVariant(tr("hello2")),
                                  QVariant(tr("hello3"))};
    QVariant tempVar = QVariant(tr("hello"));
    QVector<FeatureVectType> data_;
    using selectorType = std::function<bool(double)>;
public:
    void normalize();
    void unnormalize();
    const QVector<FeatureVectType> & getData() const;
    explicit WindowDataTableModel(QObject *parent = nullptr);
    void set(QString * inputFilename);
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void printRow(int index, std::ofstream & output) const;
    QVector<QString> columnNames();
    const QVector<double> & getMinValues() const;
    const QVector<double> & getMaxValues() const;
    QVector<size_t> getRowsBySelectors(QMap<QString, selectorType> selectors);
    bool isVectorProperty(const QString & propertyName) const;
};

#endif // WINDOWDATATABLEMODEL_H
