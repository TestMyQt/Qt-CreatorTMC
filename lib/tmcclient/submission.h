#ifndef SUBMISSION_H
#define SUBMISSION_H

#include "testcase.h"

#include <QJsonObject>
#include <QString>
#include <QList>
#include <QDate>
#include <QUrl>

class Submission
{
public:
    Submission(int id);
    bool operator==(const Submission &other) const;
    bool operator!=(const Submission &other) const;

    enum Status
    {
        Ok,
        Fail,
        Error,
        Processing,
        Hidden
    };

    void setStatus(Status status);
    void setPoints(QStringList points);
    void setTestCases(QList<TestCase> cases);
    void setPassed(bool passed);
    void setSubmissionTime(QDate time);
    void setError(QString error);
    void setSolutionUrl(QString url);

    int getId() const;
    Status getStatus() const;
    QStringList getPoints() const;
    QList<TestCase> getTestCases() const;
    bool allTestPassed() const;
    QDate submissionTime() const;
    QString errorString() const;
    QString solutionUrl() const;

    static Submission fromJson(const int id, const QJsonObject jsonSubmission);

private:

    int m_id;
    Status m_status;
    QStringList m_points;
    QList<TestCase> m_testCases;
    bool m_testsPassed;
    QDate m_submissionTime;
    QString m_error;
    QString m_solutionUrl;

};

#endif // SUBMISSION_H
