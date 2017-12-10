#ifndef TMCRUNNER_H
#define TMCRUNNER_H

#include "tmctestresult.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>

#include <QFutureWatcher>
#include <QObject>
#include <QProcess>

using namespace TestMyCode;
using ProjectExplorer::Project;

class TMCRunner : public QObject
{
    Q_OBJECT

public:
    static TMCRunner* instance();
    ~TMCRunner();

    bool isTMCRunning() const { return m_executingTMC; }
    void testProject(Project *project);

signals:
    void testResultReady(const QList<TmcTestResult> &result);
    void testsPassed(Project *passedProject);

private:
    void launchTmcCLI(const Utils::FileName &workingDirectory);
    QList<TmcTestResult> readTMCOutput(const QString &testOutput);
    bool checkPassedStatus(QList<TmcTestResult> testResults);
    ProjectExplorer::Project *m_activeProject;
    void onFinished();
    bool m_executingTMC;

    explicit TMCRunner(QObject *parent = 0);

};

#endif // TMCRUNNER_H
