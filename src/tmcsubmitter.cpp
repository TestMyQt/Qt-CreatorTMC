#include "tmcsubmitter.h"
#include "testmycodeconstants.h"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QDirIterator>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

using Core::ProgressManager;
using Core::FutureProgress;

static constexpr int update_interval = 5000; // Update every 5 seconds
static constexpr int update_retry = 30; // Update 30 times, for 2,5 minutes

TmcSubmitter::TmcSubmitter(TmcClient *client, QObject *parent) :
    QObject(parent),
    m_client(client)
{
    connect(m_client, &TmcClient::exerciseSubmitReady, this, &TmcSubmitter::onSubmitReply);
    connect(m_client, &TmcClient::submissionStatusReady, this, &TmcSubmitter::onSubmissionStatusReply);

    connect(m_client, &TmcClient::exerciseSubmitReady, &m_submit, &SubmitWidget::onSubmitReply);
    connect(m_client, &TmcClient::exerciseSubmitProgress, &m_submit, &SubmitWidget::submitProgress);
    connect(m_client, &TmcClient::submissionStatusReady, &m_submit, &SubmitWidget::updateStatus);

    m_submitTimer.setSingleShot(false);
    m_submitTimer.setInterval(update_interval);
}

TmcSubmitter::~TmcSubmitter()
{
    if (m_uploadProgress.isRunning())
        m_uploadProgress.reportFinished();

    if (m_submitProgress.isRunning())
        m_submitProgress.reportFinished();
}


void TmcSubmitter::submitProject(const ProjectExplorer::Project *project)
{
    QString dir = project->projectDirectory().toString();
    QDir projectDir(dir);
    // TODO: figure out why AllFiles does not contain .qrc files.
    FileNameList allFiles = project->files(ProjectExplorer::Project::AllFiles);
    // Need to add qrc's manually
    QDirIterator it(dir, QStringList() << "*.qrc", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        allFiles << FileName(it.fileInfo());
    }
    QBuffer *zipBuffer = new QBuffer;
    zipBuffer->open(QIODevice::ReadWrite);

    Exercise ex = project->property("exercise").value<Exercise>();
    if (!ex) return;

    qDebug() << "Zipping" << ex.getName();
    if (!createZip(projectDir, allFiles, zipBuffer)) {
        qDebug() << "Zipping failed:" << project->displayName();
        return;
    }

    m_uploadProgress.setProgressRange(0, 1);
    ProgressManager::addTask(m_uploadProgress.future(),
                             tr("Submitting exercise %1").arg(project->displayName()),
                             TestMyCodePlugin::Constants::TASK_INDEX);

    m_uploadProgress.reportStarted();
    zipBuffer->open(QIODevice::ReadOnly);
    qDebug() << "Posting:" << project->displayName();
    m_client->postExerciseZip(ex, zipBuffer->readAll());
    m_submit.show();
}

void TmcSubmitter::onSubmitReply(Exercise ex, QString submissionUrl)
{
    m_uploadProgress.reportFinished();

    QUrl subUrl(submissionUrl);
    int submissionId = subUrl.fileName().toInt();

    // TODO: timeout value of the submission?
    // For now 30 * 5 = 150 seconds or 2,5 minutes
    m_submitProgress.setProgressRange(0, update_retry);
    m_submitProgress.reportStarted();
    ProgressManager::addTask(m_submitProgress.future(),
                             tr("Getting status for %1").arg(ex.getName()),
                             TestMyCodePlugin::Constants::TASK_INDEX);

    connect(&m_submitTimer, &QTimer::timeout, this, [this, submissionId]() {
        m_client->getSubmissionStatus(submissionId);
    });
    m_submitTimer.start();
}

void TmcSubmitter::onSubmissionStatusReply(Submission sub)
{
    qDebug() << "Got submission status for" << sub.getId();

    if (sub.getStatus() != Submission::Processing) {
        m_submitTimer.stop();
        m_submitProgress.reportFinished();
    }

    int timeout = m_submitProgress.progressValue();
    if (timeout < update_retry) {
        m_submitProgress.setProgressValue(timeout + 1);
    } else {
        m_submitTimer.stop();
        m_submitProgress.reportFinished();
        emit submitTimedOut(sub);
        return;
    }

    emit submitResult(sub);
}

bool TmcSubmitter::createZip(QDir projectDir, FileNameList files, QBuffer *zipBuffer)
{
    QuaZip zip(zipBuffer);

    if (!zip.open(QuaZip::mdCreate)) {
        return false;
    }

    QuaZipFile zipFile(&zip);
    QFile inFile;

    foreach(FileName file, files) {
        QFileInfo fileInfo = file.toFileInfo();
        if (!fileInfo.isFile())
            continue;

        QString filenameRelativePath = fileInfo.filePath().remove(0, projectDir.absolutePath().length() + 1);

        inFile.setFileName(fileInfo.filePath());
        if (!inFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        QuaZipNewInfo info = QuaZipNewInfo(filenameRelativePath, fileInfo.filePath());
        if (!zipFile.open(QIODevice::WriteOnly, info)) {
            return false;
        }

        // Copy data to zip
        zipFile.write(inFile.readAll());

        // Was writing ok?
        if (zipFile.getZipError() != 0) {
            return false;
        }

        zipFile.close();

        // Was close ok?
        if (zipFile.getZipError() != 0) {
            return false;
        }

        inFile.close();
    }

    return true;
}
