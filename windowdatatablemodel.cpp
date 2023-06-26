#include "windowdatatablemodel.h"
#include <iostream>
#include <fstream>
#include <QMessageBox>

#include <QFile>
#include <QVector>
#include <QMap>
#include <functional>
WindowDataTableModel::WindowDataTableModel(QObject *parent )
    : QAbstractTableModel{parent}
{

}
QVector<QString> WindowDataTableModel::columnNames()
{
    QVector<QString> columnNames;
    for(int i = 0; i < columnCount(); i++)
    {
        columnNames.push_back(this->headerData(i, Qt::Horizontal).toString());
    }
    return columnNames;
}
void WindowDataTableModel::normalize()
{
    beginResetModel();
    normalizeData = true;
    endResetModel();
}
void WindowDataTableModel::unnormalize()
{
    beginResetModel();
    normalizeData = false;
    endResetModel();
}
void WindowDataTableModel::printRow(int index, std::ofstream & output) const
{
    output << data_[index];
}
const QVector<WindowDataTableModel::FeatureVectType> & WindowDataTableModel::getData() const
{
    return data_;
}

void WindowDataTableModel::set(QString * inputFilename)
{
    beginResetModel();
    if(!QFile::exists(*inputFilename))
    {
        QMessageBox mBox;
        mBox.setText("Input file does not exists - " + *inputFilename + "\" ");
        mBox.exec();
       //TODO
        return;
    }
    QFile window_file(*inputFilename);
    if(!window_file.open(QFile::ReadOnly))
    {
        QMessageBox mBox;
        mBox.setText("Input file cannot be read - " + *inputFilename + "\" ");
        mBox.exec();
    }
    std::ifstream window_stream(inputFilename->toStdString());
    FeatureVectType featVect;
    data_.clear();
    while(window_stream.peek() != EOF)
    {
        window_stream >> featVect;
        data_.push_back(featVect);
        std::string line;
        std::getline(window_stream, line);
        if(maxValues.size() == 0)
        {
            minValues = QVector<double>(data_[0].attribute_names().size(), std::numeric_limits<double>::max());
            maxValues = QVector<double>(data_[0].attribute_names().size(), -std::numeric_limits<double>::max());
        }
        for(int col_index = 0; col_index < data_[0].attribute_names().size() ; ++col_index)
        {
            auto attribute_name = data_[0].attribute_names()[col_index];
            if(!data_[0].is_vector(attribute_name))
            {
                double dataDouble = data_.last().get_scalar(attribute_name);
                if(maxValues[col_index] < dataDouble)
                    maxValues[col_index] = dataDouble;
                if(minValues[col_index] > dataDouble)
                    minValues[col_index] = dataDouble;
            }
        }

    }
    loaded_ = true;

    endResetModel();
}


int WindowDataTableModel::rowCount(const QModelIndex & parent) const
{
    if(!loaded_)
        return 0;
    return data_.size();
}
int WindowDataTableModel::columnCount(const QModelIndex & parent) const
{
    if(!loaded_)
        return 0;
    return data_[0].attribute_names().size();
}
QVariant WindowDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            if (orientation == Qt::Orientation::Vertical && section < rowCount())
                return QVariant(section);
            else if (section < FeatureVectType::attribute_names().size() && orientation == Qt::Orientation::Horizontal)
                return QString::fromStdString((FeatureVectType::attribute_names())[section]);
            else return QVariant();
        break;
    default:
        return QVariant();
        break;

    }

}

QVariant WindowDataTableModel::data(const QModelIndex &index, int role) const
{
    double dataDouble;
    QString dataStr;
    bool success;
    std::string target_attrib;
    switch(role)
    {
    case Qt::DisplayRole:
        if(!loaded_)
                return QVariant(index.row() + index.column());
        target_attrib = FeatureVectType::attribute_names()[index.column()];
        if(data_[index.row()].is_vector(target_attrib))
        {
            std::vector<double> vectorData = data_[index.row()].get_vector(target_attrib);

            QString vectorStr = "[";
            vectorStr.reserve(vectorData.size() * 6);
            for (int i = 0; i < vectorData.size(); ++i)
            {
                vectorStr.append(QString::number(vectorData[i]));
                vectorStr.append(" ");
            }
            vectorStr.append("]");
            return vectorStr;
        }
        else
        {
            double dataDouble = data_[index.row()].get_scalar(target_attrib);
            if(index.column() == 0)
            dataDouble /= 1000000;
            if (normalizeData)
                return (dataDouble - minValues[index.column()])/(maxValues[index.column()] - minValues[index.column()]);
            return dataDouble;
        }

        return dataStr;
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignRight;

    case Qt::CheckStateRole:
        return Qt::Unchecked;

    default:
        return QVariant();
    }

}

QVector<size_t> WindowDataTableModel::getRowsBySelectors(QMap<QString, WindowDataTableModel::selectorType> selectors)
{
    //TODO finish this below and think about data retrieval from feature vect
    QVector<size_t> selectedIndices;
    for (size_t rowIndex = 0; rowIndex < this->rowCount(); rowIndex ++)
    {
        bool shouldSelect = true;

        for (size_t attributeIndex = 0; attributeIndex < FeatureVectType::attribute_names().size(); attributeIndex ++)
        {
            auto attributeName = QString::fromStdString(FeatureVectType::attribute_names()[attributeIndex]);

            if (!data_[0].is_vector(attributeName.toStdString()))
            {
                double dataDouble = data_[rowIndex].get_scalar(attributeName.toStdString());
                if(selectors.contains(attributeName) && ! selectors[attributeName](dataDouble))
                {
                    shouldSelect = false;
                    break;
                }
            }
        }
        if(shouldSelect)
            selectedIndices.push_back(rowIndex);
        std::cout << rowIndex << std::endl;
    }

    return selectedIndices;
}
const QVector<double>& WindowDataTableModel::getMinValues() const
{
    return minValues;
}
const QVector<double>& WindowDataTableModel::getMaxValues() const
{
    return maxValues;
}
bool WindowDataTableModel::isVectorProperty(const QString & propertyName) const
{
    if (data_.size() == 0)
        return false;
    return data_[0].is_vector(propertyName.toStdString());
}
