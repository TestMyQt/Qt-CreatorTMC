#include "tmcresultreader.h"
#include "tmctestresult.h"

#include <autotest/testrunner.h>
#include <autotest/testtreemodel.h>

#include <QDebug>

using namespace Autotest::Internal;

namespace TestMyCode {

static TmcResultReader *s_instance = nullptr;

TmcResultReader *TmcResultReader::instance()
{
    if (!s_instance)
        s_instance = new TmcResultReader;
    return s_instance;
}

TmcResultReader::TmcResultReader(QObject *parent) :
    QObject(parent),
    m_tmcRegex("(qml: )?TMC:(?<testName>.+)\\.(?<point>.*)")
{
    connect(TestRunner::instance(), &TestRunner::testResultReady,
            this, &TmcResultReader::readTestResult);
    connect(TestRunner::instance(), &TestRunner::testRunFinished,
            this, &TmcResultReader::resultsReady);

}

void TmcResultReader::testProject(Project *project)
{
    if (!project) {
        qDebug() << "Testing project null!";
        return;
    }
    m_testResults.clear();
    m_project = project;
    TestRunner *runner = TestRunner::instance();
    TestTreeModel *model = TestTreeModel::instance();
    runner->setSelectedTests(model->getAllTestCases());
    runner->prepareToRunTests(TestRunMode::Run);
    emit testRunStarted();
}

void TmcResultReader::readTestResult(const TestResultPtr &result) {
    QString description = result->description();

    // TestCase is registered as a TMC test case if it has
    // the qInfo message with prefix TMC, name and points

    Result::Type resultType = result->result();
    switch (resultType) {
    case Result::MessageTestCaseStart:
        // Start a new result
        m_openResult = TmcTestResult();
        // Emit start of test cases ?
        // emit testResultReady(TmcTestResult(TmcResult::TestCaseStart));
        break;

    case Result::Pass:
        m_openResult.setResult(TmcResult::Pass);
        break;

    case Result::Fail:
        m_openResult.setResult(TmcResult::Fail);
        // Set the assert failure message as the exception message
        m_openResult.setMessage(description);
        break;

    case Result::MessageInfo: {
        // Parse function name and points
        QRegularExpressionMatch match = m_tmcRegex.match(description);
        if (match.hasMatch()) {
            QString testName = match.captured("testName");
            QString point = match.captured("point");
            m_openResult.setName(testName);
            m_openResult.addPoint(point);
        }
        break;
    }

    case Result::MessageTestCaseEnd:
        // TestCase results have ended, lets see if we have the name and points
        if (!(m_openResult.name().isEmpty() && m_openResult.points().isEmpty())) {
            m_testResults.append(m_openResult);
            emit testResultReady(m_openResult);
        }
        // Emit end ?
        // emit testResultReady(TmcTestResult(TmcResult::TestCaseEnd));
        break;

    default:
        break;
    }
}

void TmcResultReader::resultsReady() {
    // No results == something failed / no valid tests
    if (m_testResults.isEmpty()) {
        qDebug("No valid TMC results");
        return;
    }

    bool testsPassed = true;
    foreach (TmcTestResult r, m_testResults) {
        qDebug() << r.name() << r.result() << r.points();
        if (r.result() != TmcResult::Pass) {
            testsPassed = false;
        }
    }

    emit testRunFinished();

    if (testsPassed) {
        qDebug("Project tests passed");
        emit projectTestsPassed(m_project);
    }
}

} // TestMyCode
