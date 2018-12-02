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
        return QString("[%1]: PASSED, points awarded: %2").arg(m_name, points);
    }

    case TmcResult::Fail:
        return QString("[%1]: FAILED: %2").arg(m_name, m_message);

    case TmcResult::Invalid:
        if (m_name.isEmpty())
            return QString("INVALID: %1").arg(m_message);
        else
            return QString("[%1]: INVALID: %2").arg(m_name, m_message);

    case TmcResult::TestCaseStart:
        return "";

    case TmcResult::TestCaseEnd:
        return "";
    }

    return "";
}

} // TestMyCode
