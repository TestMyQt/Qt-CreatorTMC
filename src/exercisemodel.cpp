#include "exercisemodel.h"
#include "tmcclient.h"

#include <QDateTime>
#include <QApplication>
#include <QtDebug>

ExerciseModel::ExerciseModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_activeCourse(nullptr)
{
}

void ExerciseModel::onActiveCourseChanged(Course *course)
{
    beginResetModel();
    m_activeCourse = course;
    endResetModel();
}

QList<Exercise> ExerciseModel::exercises() const
{
    if (!m_activeCourse)
        return {};

    return m_activeCourse->getExercises().values();
}

void ExerciseModel::onExerciseListUpdated(Course *updatedCourse, QList<Exercise> newExercises)
{
    QString courseName(updatedCourse->getName());

    if (courseName.isEmpty()) {
        qDebug() << "Updated course name is null!";
        return;
    }
    QString saveDirectory = QString("%1/%2").arg(m_workingDir, courseName);

    beginResetModel();

    for (Exercise &ex : newExercises) {
        ex.setLocation(saveDirectory);
        Exercise found = updatedCourse->getExercise(ex);

        if (!found) {
            // Not found, new exercise
            ex.setState(Exercise::None);
            updatedCourse->addExercise(ex);

            continue;
        }

        if (found == ex) {
            // Id and checksum matches! Not a new exercise.
            if (ex.getState() != Exercise::None) {
                // Exercise has been downloaded
                newExercises.removeAll(ex);
            }
        }
    }
    endResetModel();

    // Have new exercises
    if (!newExercises.isEmpty()) {
        Q_EMIT exerciseUpdates();
    }
}

void ExerciseModel::onWorkingDirectoryChanged(const QString &workingDir)
{
    m_workingDir = workingDir;
}

void ExerciseModel::triggerDownload()
{
    for (auto &ex : m_selected) {
        QNetworkReply *reply = TmcClient::instance()->getExerciseZip(ex);
        m_downloads[reply] = exercises().indexOf(ex);
        connect(reply, &QNetworkReply::downloadProgress, this, &ExerciseModel::onProgressUpdate);
        connect(reply, &QNetworkReply::finished, this, &ExerciseModel::onDownloadFinished);
    }
}

void ExerciseModel::onTableClicked(const QModelIndex &index)
{
    if (index.isValid() && index.column() == 1) {
        const Exercise ex = exercises().at(index.row());

        if (ex.getState() == Exercise::Downloaded) {
            Q_EMIT exerciseOpen(ex);
            return;
        }

        if (m_progress[index.row()] != 0)
            return;

        qDebug() << "Downloading exercise" << ex.getName();
        m_progress[index.row()] = 1;

        QNetworkReply *reply = TmcClient::instance()->getExerciseZip(ex);
        m_downloads.insert(reply, index.row());
        Q_EMIT dataChanged(index, index);

        connect(reply, &QNetworkReply::finished, this, &ExerciseModel::onDownloadFinished);
    }
}

void ExerciseModel::onSelectAll(int state)
{
    for (auto i = 0; i < rowCount(); ++i) {
        QModelIndex modelIndex = index(i, 0);
        setData(modelIndex, state, Qt::CheckStateRole);
    }
}

void ExerciseModel::onProgressUpdate(qint64 bytesReceived, qint64 bytesTotal)
{
    int row = m_downloads[(dynamic_cast<QNetworkReply *>(QObject::sender()))];
    QModelIndex modelIndex = index(row, 1);

    if (bytesTotal == -1)
        bytesTotal = 20000;

    m_progress[row] = static_cast<qint32>((bytesReceived * 100) / bytesTotal);

    Q_EMIT dataChanged(modelIndex, modelIndex);
}

void ExerciseModel::onDownloadFinished()
{
    int row = m_downloads[(dynamic_cast<QNetworkReply *>(QObject::sender()))];
    QModelIndex modelIndex = index(row, 1);

    m_progress[row] = -1;
    Q_EMIT dataChanged(modelIndex, modelIndex);
}

QString ExerciseModel::calculateDeadline(const Exercise &ex) const
{
    QDateTime dl = ex.getDeadline();
    if (!dl.isValid())
        return "";

    qint64 deadline = QDateTime::currentDateTimeUtc().secsTo(dl);

    qint64 days = deadline / (3600 * 24);
    deadline  -= days * 3600 * 24;
    qint64 hours = (deadline / 3600);
    deadline -= hours * 3600;
    qint64 minutes = deadline / 60;

    if (days == 0 && hours == 0) {
        return QString("%1 minutes")
                .arg(QString::number(minutes));
    }

    if (days == 0) {
        return QString("%1 hours, %2 minutes")
                .arg(QString::number(hours),
                     QString::number(minutes));
    }

    return QString("%1 days, %2 hours, %3 minutes")
            .arg(QString::number(days),
                 QString::number(hours),
                 QString::number(minutes));
}

int ExerciseModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return exercises().count();
}

int ExerciseModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant ExerciseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= exercises().size() || index.row() < 0)
        return QVariant();

    if (role == Qt::CheckStateRole && index.column() == 0) {
        bool checked = m_selected.contains(exercises().at(index.row()));
        return checked ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::DisplayRole) {
        const Exercise ex = exercises().at(index.row());

        switch (index.column()) {
        case 0: {
            return ex.getName();
        }
        case 1:
            return ex.isDownloaded() ? -1 : m_progress[index.row()];
        case 2:
            return calculateDeadline(ex);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool ExerciseModel::setData(const QModelIndex &index, const QVariant &value, int role)
{    
    if (role == Qt::CheckStateRole && index.column() == 0) {
        if (value == Qt::Checked){
            m_selected.append(exercises().at(index.row()));
        } else {
            m_selected.removeAll(exercises().at(index.row()));
        }
        Q_EMIT dataChanged(index, index);
        return true;
    }

    return true;
}

QVariant ExerciseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section) {
        case 0:
            return "Exercise";
        case 1:
            return "Action";
        case 2:
            return "Deadline";
        default:
            return QVariant();
        }
    }

    if (orientation == Qt::Vertical && role == Qt::DisplayRole){
        return section;
    }

    return QVariant();
}

Qt::ItemFlags ExerciseModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}
