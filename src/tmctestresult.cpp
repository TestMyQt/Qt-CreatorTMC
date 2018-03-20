#include "tmctestresult.h"

namespace TestMyCode {

TmcTestResult::TmcTestResult()
{
}


TmcTestResult::TmcTestResult(const TmcResult::Type result)
    : m_result(result)
{
}

QString TmcTestResult::toString() const
{
    switch (m_result) {
    case TmcResult::Pass: {
        QString points = QString(" ");
        foreach (QString point, m_points) {
            points.append(QString("[ %1 ]").arg(point));
        }
        return QString("[%1]: %2, points awarded: %3").arg(m_name, "PASSED", points);
    }

    case TmcResult::Fail:
        return QString("[%1]: %2 %3").arg(m_name, "FAILED", m_message);

    case TmcResult::Invalid:
        return QString("Invalid");

    case TmcResult::TestCaseStart:
        return "";

    case TmcResult::TestCaseEnd:
        return "";
    }

    return "";
}

} // TestMyCode
