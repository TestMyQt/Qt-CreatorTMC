#ifndef TMCRESULTREADER_H
#define TMCRESULTREADER_H

#include <QList>
#include <QRegularExpression>

#include <autotest/testresult.h>
#include <projectexplorer/project.h>

#include "tmctestresult.h"

using namespace Autotest::Internal;
using namespace ProjectExplorer;

namespace TestMyCode
{

class TmcResultReader : public QObject
{
    Q_OBJECT

public:
    static TmcResultReader* instance();
    void testProject(Project *project);

Q_SIGNALS:
    void testRunStarted();
    void testResultReady(const TmcTestResult &result);
    void testRunFinished();
    void projectTestsPassed(Project *passedProject);

public Q_SLOTS:
    void readTestResult(const TestResultPtr &result);
    void resultsReady();

private:
    explicit TmcResultReader(QObject *parent = 0);
    Project *m_project;
    TmcTestResult m_openResult;
    QList<TmcTestResult> m_testResults;
    QRegularExpression m_tmcRegex;

};

} // TestMyCode

#endif // TMCRESULTREADER_H
