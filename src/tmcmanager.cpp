#include "tmcmanager.h"
#include "testmycodeconstants.h"

#include <projectexplorer/projectexplorer.h>

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QDebug>
#include <QAction>
#include <QFuture>
#include <QPushButton>
#include <QMessageBox>
#include <QMap>
#include <QBuffer>

#include <quazip/JlCompress.h>

using Core::ProgressManager;
using Core::FutureProgress;

TmcManager::TmcManager(TmcClient *client, QObject *parent) :
    QObject(parent),
    m_client(client)
{
    m_updateTimer.setSingleShot(false);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() { updateExercises(); });

    // Initialize download window
    downloadWidget = new QWidget;
    downloadform = new Ui::downloadform;
    downloadform->setupUi(downloadWidget);

    // Monitor for active project change
    using namespace ProjectExplorer;
    SessionManager *sm = SessionManager::instance();
    connect(sm, &SessionManager::startupProjectChanged,
            this, &TmcManager::onStartupProjectChanged);

    // TmcClient
    connect(m_client, &TmcClient::exerciseListReady, this, &TmcManager::handleUpdates);
    connect(m_client, &TmcClient::exerciseZipReady, this, &TmcManager::handleZip);
    connect(m_client, &TmcClient::TMCError, this, &TmcManager::displayTMCError);

    // Signal-Slot for download window
    connect(downloadform->okbutton, &QPushButton::clicked, this, &TmcManager::onDownloadOkClicked);
    connect(downloadform->cancelbutton, &QPushButton::clicked, downloadWidget, &QWidget::close);

    m_testRunner = TMCRunner::instance();
    connect(m_testRunner, &TMCRunner::TMCError, this, &TmcManager::displayTMCError);
    connect(m_testRunner, &TMCRunner::testsPassed, this, &TmcManager::askSubmit);
    m_submitter = new TmcSubmitter(m_client);
}

TmcManager::~TmcManager()
{
    if (m_updateProgress.isRunning())
        m_updateProgress.reportFinished();

    if (m_downloadProgress.isRunning())
        m_downloadProgress.reportFinished();
}

void TmcManager::setSettings(SettingsWidget *settings)
{
    m_settings = settings;
    connect(m_settings, &SettingsWidget::autoUpdateIntervalChanged, this, &TmcManager::setUpdateInterval);
    connect(m_settings, &SettingsWidget::tmcCliLocationChanged, m_testRunner, &TMCRunner::setTmcCliLocation);
    m_testRunner->setTmcCliLocation(m_settings->getTmcCliLocation());
}

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

        if (!found) {
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

void TmcManager::onStartupProjectChanged(ProjectExplorer::Project *project)
{
    m_activeProject = project;
}

Exercise TmcManager::getProjectExercise(ProjectExplorer::Project *project)
{
    QString projectName = project->displayName();
    QMap<int, Exercise> exercises = m_settings->getActiveCourse()->getExercises();
    Exercise projectExercise;

    foreach (Exercise ex, exercises.values()) {
        if (ex.getName() == projectName) {
            // TODO: We assume active project is in active courses exercise list
            // by display name only. Need to figure out a stronger relation.
            // Something like this would be better:
            // projectExercise = m_activeProject->property("exercise").value<Exercise>();
            projectExercise = ex;
        }
    }

    return projectExercise;
}

void TmcManager::testActiveProject()
{
    if (!m_activeProject) {
        QMessageBox::information(m_settings, tr("No active project"),
                                 tr("Please open a TestMyCode project"));
        return;
    }

    Exercise projectExercise = getProjectExercise(m_activeProject);

    if (!projectExercise) {
        QMessageBox::information(m_settings, tr("Not a TestMyCode project"),
                                 tr("Please select a TestMyCode exercise"));
        return;
    }

    m_activeProject->setProperty("exercise", QVariant::fromValue(projectExercise));
    m_testRunner->testProject(m_activeProject);
}

void TmcManager::askSubmit(const ProjectExplorer::Project *project)
{
    if (!project) {
        qDebug() << "Submission project was null";
        return;
    }

    int ret = QMessageBox::question(m_settings,
                                  tr("Submit exercise to server"),
                                  tr("All tests passed!\nSubmit exercise to server?"));

    if (ret == QMessageBox::Yes) {
        m_submitter->submitProject(project);
    }
}

void TmcManager::submitActiveExercise()
{
    if (!m_activeProject) {
        QMessageBox::information(m_settings, tr("No active project"),
                                 tr("Please open a TestMyCode project"));
        return;
    }

    Exercise projectExercise = getProjectExercise(m_activeProject);
    m_activeProject->setProperty("exercise", QVariant::fromValue(projectExercise));
    m_submitter->submitProject(m_activeProject);
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
        displayTMCError("Error unzipping exercise files!");
        return;
    }

    if (!activeCourse->hasExercise(ex)) {
        displayTMCError("Exercise not found in course!");
        return;
    }

    ex.setDownloaded(true);
    ex.setUnzipped(true);
    // Save updated exercise back to course exercise list
    activeCourse->addExercise(ex);
    // open project
    using namespace ProjectExplorer;
    QString exerciseLocation =  saveDir + "/" + ex.getName() + "/" + ex.getName() + ".pro";
    const ProjectExplorerPlugin::OpenProjectResult openProjectSucceeded =
            ProjectExplorer::ProjectExplorerPlugin::openProject(exerciseLocation);
    qDebug() << "Opened:" << openProjectSucceeded.project()->displayName();
}

void TmcManager::updateExercises()
{
    Course* activeCourse = m_settings->getActiveCourse();
    if (!activeCourse || !(*activeCourse)) {
        m_settings->display();
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
        item->setCheckState(Qt::Checked);
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

void TmcManager::displayTMCError(QString errorText)
{
    QMessageBox::critical(m_settings, "TMC", errorText, QMessageBox::Ok);
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
