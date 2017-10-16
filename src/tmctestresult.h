#ifndef TMCTESTRESULT_H
#define TMCTESTRESULT_H

#include <QMetaType>
#include <QList>
#include <QString>

class TmcTestResult
{
public:
    TmcTestResult();
    explicit TmcTestResult(const QString &name);

    virtual ~TmcTestResult() {}

    QString name() const { return m_name; }
    bool isSuccessful() const { return m_successful; }
    QString message() const { return m_message; }
    QList<QString> points() const { return m_points; }
    QString exception() const { return m_exception; }

    void setMessage(const QString &message) { m_message = message; }
    void setSuccess(const bool &successful) { m_successful = successful; }
    void setPoints(QList<QString> points) { m_points = points; }
    void setException(const QString &ex) { m_exception = ex; }

private:

    QString m_name;
    bool m_successful;
    QString m_message;
    QList<QString> m_points;
    QString m_exception;

};

Q_DECLARE_METATYPE(TmcTestResult)
#endif // TMCTESTRESULT_H

