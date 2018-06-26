#ifndef EXERCISEMODEL_H
#define EXERCISEMODEL_H

#include "exercise.h"

#include <QAbstractTableModel>
#include <QList>
#include <QNetworkReply>

class ExerciseModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    ExerciseModel(QObject *parent = nullptr);
    QList<Exercise> selectedExercises();
    void triggerDownload();

    // QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool insertColumns(int column, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeColumns(int column, int count, const QModelIndex &parent) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

public slots:
    void addExercises(const QList<Exercise> exercises);
    void onSelectAll(int state);

    void onProgressUpdate(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();

private:
    QList<Exercise> m_exercises;
    QList<Exercise> m_selected;
    QMap<QNetworkReply *, int> m_exerciseIndexes;
    QMap<int, int> m_progress;

    QString calculateDeadline(const Exercise &ex) const;
};

#endif // EXERCISEMODEL_H
