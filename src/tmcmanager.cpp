/*!
    \class TmcManager
    \inmodule src
    \inheaderfile tmcmanager.h
    \brief The \l TmcManager class provides a number of mechanisms for managing and
        executing the basic operations of a QtCreatorTMC plugin session.

    The \l TmcManager object includes an internal timer that is used for periodically
    checking for exercise updates by a call to the slot \l {TmcManager::}
    {updateExercises()}. If exercise updates are found, a call to \l {TmcManager::}
    {handleUpdates()} ensues.
*/

#include "tmcmanager.h"
#include "testmycodeconstants.h"
#include "ziphelper.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QDebug>
#include <QDesktopServices>
#include <QAction>
#include <QFuture>
#include <QPushButton>
#include <QMessageBox>
#include <QMap>

using Core::ProgressManager;
using Core::FutureProgress;

TmcManager::TmcManager(TmcClient *client, QObject *parent) :
    QObject(parent),
    m_client(client)
{
    m_updateTimer.setSingleShot(false);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() { updateExercises(); });

    // Exercise list
    m_exerciseWidget = new ExerciseWidget;
    connect(m_exerciseWidget, &ExerciseWidget::onExercisesSelected,
            this, &TmcManager::downloadSelectedExercises);

    // Monitor for active project change
    using namespace ProjectExplorer;
    SessionManager *sm = SessionManager::instance();
    connect(sm, &SessionManager::startupProjectChanged,
            this, &TmcManager::onStartupProjectChanged);

    // TmcClient
    connect(m_client, &TmcClient::exerciseListReady, this, &TmcManager::handleUpdates);
    connect(m_client, &TmcClient::exerciseZipReady, this, &TmcManager::handleZip);
    connect(m_client, &TmcClient::TMCError, this, &TmcManager::displayTMCError);

    // Autotest
    connect(TmcResultReader::instance(), &TmcResultReader::projectTestsPassed,
            this, &TmcManager::askSubmit);
}

TmcManager::~TmcManager()
{
    if (m_updateProgress.isRunning())
        m_updateProgress.reportFinished();

    if (m_downloadProgress.isRunning())
        m_downloadProgress.reportFinished();
}

/*!
    Initializes the \l TmcManager object's \l SettingsWidget pointer field to \a settings.
    A \l {http://doc.qt.io/qt-5/qobject.html#connect} {connection} is established between
    signal \l {SettingsWidget::} {autoUpdateIntervalChanged} and public slot
    \l {TmcManager::} {setUpdateInterval()}.
*/
void TmcManager::setSettings(SettingsWidget *settings)
{
    m_settings = settings;
    connect(m_settings, &SettingsWidget::autoUpdateIntervalChanged, this, &TmcManager::setUpdateInterval);
    setUpdateInterval(m_settings->getAutoupdateInterval());
}

/*!
    The slot is connected to \l TmcClient::exerciseListReady. Parameter \a newExercises
    is an up-to-date version of the TMC exercises of course \a updatedCourse. Each
    \l Exercise object in \a newExercises is examined against the already present
    exercises in \a updatedCourse. Any exercise in \a newExercises that is found to already
    be in \a updatedCourse is removed from \a newExercises. After removing the already
    present exercises from \a newExercises the list is then used to inform the user of
    exercise updates (if any are available).
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

    // Have new exercises
    if (!newExercises.isEmpty()) {
        showExerciseWidget(newExercises);
    }

    m_updateProgress.reportFinished();
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

    foreach (const Exercise &ex, exercises) {
        QStringList parts = ex.getName().split("-");
        QString exerciseName = parts.last();
        if (exerciseName == projectName) {
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
    TmcResultReader::instance()->testProject(m_activeProject);
}

void TmcManager::askSubmit(const ProjectExplorer::Project *project)
{
    if (!project) {
        qDebug() << "Submission project was null";
        return;
    }

    if (!m_client->isAuthenticated() || !m_settings->getActiveCourse())
        return;

    int ret = QMessageBox::question(m_settings,
                                  tr("Submit exercise to server"),
                                  tr("All tests passed!\nSubmit exercise to server?"));

    if (ret == QMessageBox::Yes) {
        showSubmitWidget(project);
    }
}

void TmcManager::showSubmitWidget(const Project *project)
{
    auto *submit = new SubmitWidget();
    submit->show();
    connect(m_client, &TmcClient::exerciseSubmitReady, submit, &SubmitWidget::onSubmitReply);
    connect(m_client, &TmcClient::exerciseSubmitProgress, submit, &SubmitWidget::submitProgress);
    connect(m_client, &TmcClient::submissionStatusReady, submit, &SubmitWidget::updateStatus);
    connect(submit, &SubmitWidget::projectSubmissionReady, m_client, &TmcClient::postExerciseZip);
    connect(submit, &SubmitWidget::submissionStatusRequest, m_client, &TmcClient::getSubmissionStatus);
    submit->setAttribute(Qt::WA_DeleteOnClose);
    submit->submitProject(project);
}

void TmcManager::showExerciseWidget(QList<Exercise> exercises)
{
    m_exerciseWidget->addExercises(exercises);
    m_exerciseWidget->show();
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
    showSubmitWidget(m_activeProject);
}

void TmcManager::openActiveCoursePage()
{
    auto activeCourse = m_settings->getActiveCourse();
    if (!activeCourse)
        return;

    QSettings settings("TestMyQt", "TMC");
    QString serverUrl = settings.value("server",
                                       TestMyCodePlugin::Constants::DEFAULT_TMC_SERVER).toString();
    QString activeCourseId = QString::number(activeCourse->getId());

    QString courseUrl = QString("%1/courses/%2").arg(serverUrl, activeCourseId);
    QDesktopServices::openUrl(QUrl(courseUrl));

    settings.deleteLater();
}

void TmcManager::handleZip(QByteArray zipData, Exercise ex)
{
    Course *activeCourse = m_settings->getActiveCourse();

    QString saveDir = QString("%1/%2").arg(m_settings->getWorkingDirectory(),
                                           activeCourse->getName());

    QStringList extracted = ZipHelper::extractZip(&zipData, saveDir);
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

    ex.setLocation(saveDir);
    // Save updated exercise back to course exercise list
    activeCourse->addExercise(ex);
    QSettings settings("TestMyQt", "TMC");
    ex.saveQSettings(&settings, activeCourse->getName());
    settings.deleteLater();
    // open project
    openExercise(ex);
}

void TmcManager::downloadSelectedExercises(QList<Exercise> selected)
{
    qDebug() << "Download exercises";
}

void TmcManager::openExercise(Exercise &ex)
{
    if (ex.getLocation().isEmpty()) {
        displayTMCError("Exercise location is empty!");
        return;
    }
    using namespace ProjectExplorer;

    QString proFile = ex.getProFile();
    const auto openProject = ProjectExplorerPlugin::openProject(proFile);

    if (!openProject.alreadyOpen().isEmpty()) {
        qDebug() << "Project already open";
        return;
    }

    if (!openProject) {
        qDebug() << "Exercise open not successful: " << proFile;
        displayTMCError("Exercise open not successful: " + proFile);
        return;
    }

    qDebug() << "Opened:" << openProject.project()->displayName();
}

/*!
    Called periodically to refresh the TMC exercise list of the
    \l {SettingsWidget::getActiveCourse()} {active course}. The purpose is to determine
    whether new or updated exercises are available.
*/
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

/*!
    Sets the \l {TmcManager::updateInterval()} {update interval} of the \l TmcManager
    object to \a interval seconds.
*/
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

