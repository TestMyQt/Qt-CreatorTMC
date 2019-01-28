#ifndef EXERCISEMODEL_H
#define EXERCISEMODEL_H

#include "course.h"
#include "exercise.h"

#include <QAbstractTableModel>
#include <QList>
#include <QNetworkReply>

class ExerciseModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ExerciseModel(QObject *parent = nullptr);

    void onActiveCourseChanged(Course *course);
    QList<Exercise> exercises() const;

    void triggerDownload();
    void onTableClicked(const QModelIndex &index);

    // QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

public Q_SLOTS:
    void onExerciseListUpdated(Course *updatedCourse, QList<Exercise> newExercises);
    void onWorkingDirectoryChanged(const QString &workingDir);
    void onSelectAll(int state);

    void onProgressUpdate(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();

Q_SIGNALS:
    void exerciseUpdates();
    void exerciseOpen(const Exercise &ex);

private:
    Course *m_activeCourse;
    QList<Exercise> m_selected;
    QMap<QNetworkReply *, int> m_downloads;
    QMap<int, int> m_progress;

    QString m_workingDir;

    QString calculateDeadline(const Exercise &ex) const;
};

#endif // EXERCISEMODEL_H
