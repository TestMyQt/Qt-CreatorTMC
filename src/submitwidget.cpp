#include "submitwidget.h"
#include "testcase.h"
#include "testmycodeconstants.h"
#include "ziphelper.h"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

using Core::ProgressManager;
using Core::FutureProgress;

static constexpr int update_interval = 5000; // Update every 5 seconds
static constexpr int update_retry = 30; // Update 30 times, for 2,5 minutes

SubmitWidget::SubmitWidget(QWidget *parent) : QWidget(parent)
{
    m_submitWindow = new Ui::Submit;
    m_submitWindow->setupUi(this);

    m_status = m_submitWindow->status;
    m_progressBar = m_submitWindow->progressBar;
    m_progressBar->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 0px; text-align: center; }"
                                 "QProgressBar::chunk { background-color: #bcbcbc; width: 2px; }");
    m_cancel = m_submitWindow->cancel->button(QDialogButtonBox::Cancel);

    m_submitTimer.setSingleShot(false);
    m_submitTimer.setInterval(update_interval);

    connect(m_cancel, &QPushButton::clicked, this, &QWidget::close);
}

SubmitWidget::~SubmitWidget()
{
    if (m_uploadProgress.isRunning())
        m_uploadProgress.reportFinished();

    if (m_submitProgress.isRunning())
        m_submitProgress.reportFinished();
}

void SubmitWidget::submitProject(const Project *project)
{
    Exercise ex = project->property("exercise").value<Exercise>();
    if (!ex)
        return;

    QString dir = project->projectDirectory().toString();
    QDir projectDir(dir);
    // TODO: figure out why AllFiles does not contain .qrc files.
    FileNameList allFiles = project->files(Project::AllFiles);
    // Need to add qrc's manually
    QDirIterator it(dir, QStringList() << "*.qrc", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        FileName name = FileName(it.fileInfo());
        if (!allFiles.contains(name)) {
            allFiles << name;
        }
    }
    QBuffer *zipBuffer = new QBuffer;
    zipBuffer->open(QIODevice::ReadWrite);

    qDebug() << "Zipping" << ex.getName();
    if (!ZipHelper::createZip(projectDir, allFiles, zipBuffer)) {
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
    emit projectSubmissionReady(ex, zipBuffer->readAll());
}

void SubmitWidget::onSubmitReply(Exercise ex, QString submissionUrl)
{
    setWindowTitle("Submitting " + ex.getName());
    m_status->setText("Submitting " + ex.getName());

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
        emit submissionStatusRequest(submissionId);
    });
    m_submitTimer.start();
}

void SubmitWidget::onSubmissionStatusReply(Submission sub)
{
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

void SubmitWidget::submitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal)
{
    this->setWindowTitle("Submitting " + ex.getName());
    m_status->setText("Submitting " + ex.getName());
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(static_cast<qint32>((bytesSent * 100) / bytesTotal));
}

void SubmitWidget::updateStatus(Submission submission)
{
    QString output;
    switch (submission.getStatus()) {
    case (Submission::Error):
        m_status->setText("Submission processing failed:\n"
                          + submission.errorString());
        m_progressBar->hide();
        break;
    case (Submission::Processing):
        m_status->setText("Processing...");
        m_progressBar->setMaximum(0);
        m_progressBar->setMinimum(0);
        break;
    case (Submission::Fail):
        foreach (TestCase testCase, submission.getTestCases()) {
            output.append(QString("%1: %2\n")
                          .arg(testCase.name, testCase.message));
        }
        m_status->setText("Tests did not pass on server:\n"
                          "Testcases:\n" + output);
        m_progressBar->hide();
        break;
    case (Submission::Ok):
        m_status->setText(QString("<p>All tests passed!</p>"
                          "<p>Points awarded: %1</p>"
                          "<a href=\"%2\">Click here for the suggested solution</a>")
                          .arg(submission.getPoints().join(", "),
                               submission.solutionUrl()));
        m_status->setTextFormat(Qt::RichText);
        m_status->setTextInteractionFlags(Qt::TextBrowserInteraction);
        m_status->setOpenExternalLinks(true);
        m_progressBar->hide();

        break;
    case (Submission::Hidden):
        // ????
        break;
    }
}
