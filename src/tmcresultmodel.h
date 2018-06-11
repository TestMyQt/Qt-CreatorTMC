#ifndef TMCRESULTMODEL_H
#define TMCRESULTMODEL_H

#include <QAbstractItemModel>
#include <QAbstractListModel>

#include <QIcon>

#include "tmctestresult.h"

using namespace TestMyCode;

class TmcResultModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TmcResultModel(QObject *parent = nullptr);

    // QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    TmcTestResult testResult(const QModelIndex &idx) const;
    QList<TmcTestResult> results() const;
    void addResult(const TmcTestResult &result);
    void addResults(const QList<TmcTestResult> &results);
    void clearResults();
    bool hasResults();

private:

    QList<TmcTestResult> m_results;
};

#endif // TMCRESULTMODEL_H
