/*!
    \class TmcManager
    \inmodule src
    \inheaderfile tmcmanager.h
    \brief TODO: QDoc
*/

#include "tmcmanager.h"
#include "testmycodeconstants.h"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QDebug>
#include <QAction>
#include <QFuture>
#include <QPushButton>
#include <QMap>
#include <QBuffer>

#include <quazip/JlCompress.h>

using Core::ProgressManager;
using Core::FutureProgress;

TmcManager::TmcManager(QObject *parent) : QObject(parent)
{
    m_updateTimer.setSingleShot(false);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() { updateExercises(); });

    // Initialize download window
    downloadWidget = new QWidget;
    downloadform = new Ui::downloadform;
    downloadform->setupUi(downloadWidget);

    // Signal-Slot for download window
    connect(downloadform->okbutton, &QPushButton::clicked, this, &TmcManager::onDownloadOkClicked);
    connect(downloadform->cancelbutton, &QPushButton::clicked, this, [=](){
        downloadWidget->close();
    });
}

TmcManager::~TmcManager()
{
    if (m_updateProgress.isRunning())
        m_updateProgress.reportFinished();

    if (m_downloadProgress.isRunning())
        m_downloadProgress.reportFinished();
}

void TmcManager::setTmcClient(TmcClient *client)
{
    m_client = client;
    connect(m_client, &TmcClient::exerciseListReady, this, &TmcManager::handleUpdates);
    connect(m_client, &TmcClient::exerciseZipReady, this, &TmcManager::handleZip);
}

void TmcManager::setSettings(SettingsWidget *settings)
{
    m_settings = settings;
    connect(m_settings, &SettingsWidget::autoUpdateIntervalChanged, this, &TmcManager::setUpdateInterval);
}

/*!
    TODO: QDoc
*/
void TmcManager::handleUpdates(Course *updatedCourse, QList<Exercise> newExercises)
{
    QString workingDirectory = m_settings->getWorkingDirectory();
    QString courseName = updatedCourse->getName();

    if (workingDirectory.isEmpty()) {
        qDebug() << "No save directory set!";
        return;
    }

    if (updatedCourse->getName().isEmpty()) {
        qDebug() << "Updated course name is null!";
        return;
    }

    QString saveDirectory = QString("%1/%2").arg(workingDirectory, courseName);

    foreach (Exercise ex, newExercises) {
        ex.setLocation(saveDirectory);

        Exercise found = updatedCourse->getExercise(ex);

        if (found.getId() == -1) {
            // Not found, new exercise
            ex.setDownloaded(false);
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

    downloadform->exerciselist->clear();
    appendToDownloadWindow(newExercises);
    m_updateProgress.reportFinished();
    downloadWidget->show();
}

void TmcManager::handleZip(QByteArray zipData, Exercise ex)
{
    Course *activeCourse = m_settings->getActiveCourse();

    QString saveDir = QString("%1/%2").arg(m_settings->getWorkingDirectory(),
                                           activeCourse->getName());

    QBuffer zipBuffer(&zipData);
    zipBuffer.open(QIODevice::ReadOnly);

    QStringList extracted = JlCompress::extractDir(&zipBuffer, saveDir);
    if (extracted.isEmpty()) {
        emit TMCError("Error unzipping exercise files!");
        return;
    }

    if (!activeCourse->hasExercise(ex)) {
        emit TMCError("Exercise not found in course!");
        return;
    }

    ex.setDownloaded(true);
    ex.setUnzipped(true);
    // Save updated exercise back to course exercise list
    activeCourse->addExercise(ex);
}

void TmcManager::updateExercises()
{
    Course* activeCourse = m_settings->getActiveCourse();
    if (!activeCourse) {
        qDebug() << "No active course!";
        return;
    }

    qDebug() << "Updating exercises for course" << activeCourse->getName();
    m_updateProgress.setProgressRange(0, activeCourse->getExercises().size());
    ProgressManager::addTask(m_updateProgress.future(),
                             tr("Updating TestMyCode exercises"),
                             TestMyCodePlugin::Constants::TASK_INDEX);
    m_updateProgress.reportStarted();
    m_client->getExerciseList(activeCourse);
}

void TmcManager::appendToDownloadWindow(QList<Exercise> exercises)
{
    foreach (Exercise ex, exercises) {
        QListWidgetItem* item = new QListWidgetItem(ex.getName(), downloadform->exerciselist);
        item->setData(Qt::UserRole, QVariant::fromValue(ex));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked);
    }
}

bool TmcManager::lastUpdateSuccessful()
{
    return m_updateSuccessful;
}

void TmcManager::setUpdateInterval(int interval)
{
    // Interval in minutes
    m_updateTimer.setInterval(interval * 60000);
    if (m_updateTimer.interval() > 0)
        m_updateTimer.start();
}

void TmcManager::showDownloadWidget()
{
    if (!m_client->isAuthenticated()) {
        m_settings->showLoginWidget();
        return;
    }
    updateExercises();
    downloadWidget->show();
}

void TmcManager::onDownloadOkClicked()
{
    auto exerciseList = downloadform->exerciselist;
    downloadPanel = new DownloadPanel();

    for (int idx = 0; idx < exerciseList->count(); idx++) {
        auto item = exerciseList->item(idx);
        if (item->checkState() == Qt::Checked) {
            Exercise ex = item->data(Qt::UserRole).value<Exercise>();
            downloadPanel->addWidgetsToDownloadPanel(ex.getName());
            QNetworkReply* reply = m_client->getExerciseZip(ex);

            connect(reply, &QNetworkReply::downloadProgress,
                downloadPanel, &DownloadPanel::networkReplyProgress);
            connect(reply, &QNetworkReply::finished,
                downloadPanel, &DownloadPanel::httpFinished);

            downloadPanel->addReplyToList(reply);
        }
    }

    downloadPanel->addInfoLabel();
    downloadPanel->sanityCheck(); // Should be removed at some point
    downloadPanel->show();
    downloadWidget->close();
}
