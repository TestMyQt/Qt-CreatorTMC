#include "tmcresultmodel.h"
#include "tmctestresult.h"

#include <QStandardItemModel>
#include <QDebug>

TmcResultModel::TmcResultModel(QObject *parent) : QAbstractListModel(parent)
{

}

TmcTestResult TmcResultModel::testResult(const QModelIndex &idx) const
{
    int row = idx.row();
    if (!idx.isValid() || row < 0 || row >= m_results.count())
        return TmcTestResult();

    return m_results.at(row);
}


QList<TmcTestResult> TmcResultModel::results() const
{
    return m_results;
}

void TmcResultModel::addResult(const TmcTestResult &result)
{
    int i = m_results.count();
    beginInsertRows(QModelIndex(), i, i);
    m_results.insert(m_results.end(), result);
    endInsertRows();
}

void TmcResultModel::addResults(const QList<TmcTestResult> &results)
{
    int i = m_results.count();
    beginInsertRows(QModelIndex(), i, results.count());
    m_results.append(results);
    endInsertRows();
}

void TmcResultModel::clearResults()
{
    if (m_results.empty())
        return;
    beginRemoveRows(QModelIndex(), 0, m_results.count() - 1);
    m_results.erase(m_results.begin(), m_results.end());
    endRemoveRows();
}

bool TmcResultModel::hasResults()
{
    return !m_results.isEmpty();
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
    if (role == Qt::DisplayRole) {
        return m_results.at(row).toString();
    }

    return QVariant();
}
