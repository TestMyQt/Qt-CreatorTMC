#include "exercisemodel.h"
#include "tmcclient.h"

#include <QDebug>
#include <QDateTime>

ExerciseModel::ExerciseModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void ExerciseModel::onExerciseListUpdated(Course *updatedCourse, QList<Exercise> newExercises)
{
    QString courseName = updatedCourse->getName();

    if (updatedCourse->getName().isEmpty()) {
        qDebug() << "Updated course name is null!";
        return;
    }

    QString saveDirectory = QString("%1/%2").arg(m_workingDir, courseName);

    for (Exercise &ex : newExercises) {
        ex.setLocation(saveDirectory);

        Exercise found = updatedCourse->getExercise(ex);

        if (!found) {
            // Not found, new exercise
            ex.setDownloaded(false);
            ex.setUnzipped(false);
            updatedCourse->addExercise(ex);
            continue;
        }

        if (found == ex) {
            // Id and checksum matches! Not a new exercise.
            if (found.isDownloaded()) {
                // Exercise has been downloaded
                newExercises.removeAll(ex);
            }
        }
    }

    // Have new exercises
    if (!newExercises.isEmpty()) {
        emit exerciseUpdates();
    }

    beginResetModel();
    m_exercises = updatedCourse->getExercises().values();
    endResetModel();
}

void ExerciseModel::onWorkingDirectoryChanged(QString workingDir)
{
    m_workingDir = workingDir;
}

void ExerciseModel::triggerDownload()
{
    for (auto &ex : m_selected) {
        QNetworkReply* reply = TmcClient::instance()->getExerciseZip(ex);
        m_downloads[reply] = ex;
        connect(reply, &QNetworkReply::downloadProgress, this, &ExerciseModel::onProgressUpdate);
        connect(reply, &QNetworkReply::finished, this, &ExerciseModel::onDownloadFinished);
    }
}

void ExerciseModel::onTableClicked(const QModelIndex &index)
{
    if (index.isValid() && index.column() == 1) {
        const Exercise &ex = m_exercises.at(index.row());

        if (ex.isDownloaded() && ex.isUnzipped()) {
            emit exerciseOpen(ex);
            return;
        }

        qDebug() << "Downloading exercise" << ex.getName();
        QNetworkReply* reply = TmcClient::instance()->getExerciseZip(ex);
        m_downloads[reply] = ex;
        m_progress[ex] = 0;
        connect(reply, &QNetworkReply::downloadProgress, this, &ExerciseModel::onProgressUpdate);
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
    Exercise &ex = m_downloads[(static_cast<QNetworkReply *>(QObject::sender()))];
    QModelIndex modelIndex = index(m_exercises.indexOf(ex), 1);

    if (bytesTotal == -1)
        bytesTotal = 1500000;

    m_progress[ex] = static_cast<qint32>((bytesReceived * 100) / bytesTotal);

    emit dataChanged(modelIndex, modelIndex);
}

void ExerciseModel::onDownloadFinished()
{
    Exercise &ex = m_downloads[(static_cast<QNetworkReply *>(QObject::sender()))];
    QModelIndex modelIndex = index(m_exercises.indexOf(ex), 1);

    m_progress[ex] = -1;
    emit dataChanged(modelIndex, modelIndex);
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
    return m_exercises.count();
}

int ExerciseModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant ExerciseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= m_exercises.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::CheckStateRole && index.column() == 0) {
        bool checked = m_selected.contains(m_exercises.at(index.row()));
        return checked ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::DisplayRole) {
        const Exercise& ex = m_exercises.at(index.row());
        switch (index.column()) {
        case 0:
            return ex.getName();
        case 1:
            return ex.isUnzipped() ? -1 : m_progress[ex];
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
            m_selected.append(m_exercises.at(index.row()));
        } else {
            m_selected.removeAll(m_exercises.at(index.row()));
        }
        emit dataChanged(index, index);
        return true;
    }

    return true;
}

bool ExerciseModel::insertRows(int row, int count, const QModelIndex &)
{
    Q_UNUSED(row)
    Q_UNUSED(count)
    return true;
}

bool ExerciseModel::removeRows(int row, int count, const QModelIndex &){
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        m_exercises.removeAt(row);
    endRemoveRows();
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
