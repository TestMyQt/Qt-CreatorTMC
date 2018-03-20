#ifndef TMCTESTRESULT_H
#define TMCTESTRESULT_H

#include <QMetaType>
#include <QList>
#include <QString>

namespace TestMyCode {

namespace TmcResult {
enum Type {
    Pass, FIRST_TYPE = Pass,
    Fail,
    Invalid,
    TestCaseStart,
    TestCaseEnd,
    LAST_TYPE = Invalid
};
}

class TmcTestResult
{
public:
    TmcTestResult();
    explicit TmcTestResult(const TmcResult::Type result);

    virtual ~TmcTestResult() {}

    QString name() const { return m_name; }
    TmcResult::Type result() const { return m_result; }
    QString message() const { return m_message; }
    QList<QString> points() const { return m_points; }
    QString exception() const { return m_exception; }
    QString toString() const;

    void setName(const QString &name) { m_name = name; }
    void setMessage(const QString &message) { m_message = message; }
    void setResult(const TmcResult::Type result) { m_result = result; }
    void addPoint(const QString point) { m_points.append(point); }
    void setException(const QString &ex) { m_exception = ex; }

private:

    QString m_name;
    TmcResult::Type m_result = TmcResult::Invalid;
    QString m_message;
    QStringList m_points;
    QString m_exception;

};

} // TestMyCode

Q_DECLARE_METATYPE(TestMyCode::TmcTestResult)
#endif // TMCTESTRESULT_H

