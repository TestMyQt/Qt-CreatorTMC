#include "tmcresultmodel.h"
#include "tmctestresult.h"

#include <utils/qtcassert.h>
#include <QStandardItemModel>

TmcResultModel::TmcResultModel(QObject *parent) : QAbstractItemModel(parent)
{

}

void TmcResultModel::addResult(const TmcTestResult &result)
{
    int i = m_results.count();
    beginInsertRows(QModelIndex(), i, i);
    m_results.insert(m_results.end(), result);
    endInsertRows();
}

void TmcResultModel::clearResults()
{
    int index = 0;
    int start = 0;
    while (index < m_results.count()) {
        while (index < m_results.count()) {
            ++start;
            ++index;
        }
        if (index == m_results.count())
            break;
        while (index < m_results.count())
            ++index;

        beginRemoveRows(QModelIndex(), start, index - 1);
        m_results.erase(m_results.begin() + start, m_results.begin() + index);

        endRemoveRows();
        index = start;
    }
}

QModelIndex TmcResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex TmcResultModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int TmcResultModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_results.count();
}

int TmcResultModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant TmcResultModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row < 0 || row >= m_results.count() || index.column() != 0)
        return QVariant();

    return QVariant::fromValue(result(index));
}

TmcTestResult TmcResultModel::result(const QModelIndex &index) const
{
    int row = index.row();
    if (!index.isValid() || row < 0 || row >= m_results.count())
        return TmcTestResult();
    return m_results.at(row);
}
