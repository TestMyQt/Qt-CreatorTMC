#ifndef TMCSUBMITTER_H
#define TMCSUBMITTER_H

#include "tmcclient.h"
#include "submitwidget.h"

#include <QFutureInterface>
#include <QTimer>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>

using ProjectExplorer::Project;

class TmcSubmitter : public QObject
{
    Q_OBJECT
public:
    explicit TmcSubmitter(TmcClient *client, QObject *parent = nullptr);
    ~TmcSubmitter();

    void submitProject(const Project *project);
    bool createZip(QDir projectDir, QStringList files, QBuffer *zipBuffer);

signals:
    void submitResult(Submission sub);
    void submitTimedOut(Submission sub);

public slots:
    void onSubmitReply(Exercise ex, QString submissionUrl);
    void onSubmissionStatusReply(Submission sub);

private:
    TmcClient *m_client;
    SubmitWidget m_submit;

    QTimer m_submitTimer;

    QFutureInterface<void> m_uploadProgress;
    QFutureInterface<void> m_submitProgress;
};

#endif // TMCSUBMITTER_H
