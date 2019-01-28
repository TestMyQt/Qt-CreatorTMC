#ifndef SUBMITWIDGET_H
#define SUBMITWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "ui_submit.h"
#include "exercise.h"

#include <QFutureInterface>
#include <QTimer>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>

#include <utils/fileutils.h>

using ProjectExplorer::Project;
using Utils::FileName;
using Utils::FileNameList;

class SubmitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubmitWidget(QWidget *parent = nullptr);
    ~SubmitWidget();

    void submitProject(const Project *project);

Q_SIGNALS:
    void projectSubmissionReady(Exercise ex, QByteArray zipData);
    void submissionStatusRequest(int submissionId);
    void submitResult(Submission sub);
    void submitTimedOut(Submission sub);

public Q_SLOTS:
    void onSubmitReply(Exercise ex, QString submissionUrl);
    void onSubmissionStatusReply(Submission sub);
    void submitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal);
    void updateStatus(Submission submission);


private:
    Ui::Submit *m_submitWindow;

    QLabel *m_status;
    QProgressBar *m_progressBar;
    QPushButton *m_cancel;

    QTimer m_submitTimer;

    QFutureInterface<void> m_uploadProgress;
    QFutureInterface<void> m_submitProgress;

};

#endif // SUBMITWIDGET_H
