#ifndef TMCRESULTMODEL_H
#define TMCRESULTMODEL_H

#include <QAbstractItemModel>

#include <QIcon>

#include "tmctestresult.h"

class TmcResultModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TmcResultModel(QObject *parent);

    // QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    TmcTestResult result(const QModelIndex &index) const;

    QList<TmcTestResult> results() const;
    void addResult(const TmcTestResult &result);
    void addResults(const QList<TmcTestResult> &results);
    void clearResults();

private:

    QList<TmcTestResult> m_results;
};

#endif // TMCRESULTMODEL_H
