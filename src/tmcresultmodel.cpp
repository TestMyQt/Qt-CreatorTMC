#include "tmcresultmodel.h"
#include "tmctestresult.h"

#include <utils/qtcassert.h>
#include <QStandardItemModel>
#include <QDebug>

TmcResultModel::TmcResultModel(QObject *parent) : QAbstractListModel(parent)
{

}

void TmcResultModel::addResult(const TmcTestResult &result)
{
    int i = m_results.count();
    beginInsertRows(QModelIndex(), i, i);
    m_results.insert(m_results.end(), result);
    qDebug() << "adding " << result.name();
    qDebug() << "result size " << m_results.count();
    endInsertRows();
}

void TmcResultModel::clearResults()
{

    beginRemoveRows(QModelIndex(), 0, m_results.count() - 1);
    m_results.erase(m_results.begin(), m_results.end());
    endRemoveRows();

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
        const TmcTestResult r = result(index);

        if (r.isSuccessful()) {
            QString points = QString(" ");
            foreach (QString point, r.points()) {
                points.append("[ ");
                points.append(point);
                points.append(" ] ");
            }
            return QVariant(QString("[%1]: %2, points awarded: %3").arg(r.name(), "PASSED", points));
        }
        return QVariant(QString("[%1]: %2").arg(r.name(), r.message()));

    }

    return QVariant();
}

TmcTestResult TmcResultModel::result(const QModelIndex &index) const
{
    int row = index.row();
    if (!index.isValid() || row < 0 || row >= m_results.count())
        return TmcTestResult();
    return m_results.at(row);
}
