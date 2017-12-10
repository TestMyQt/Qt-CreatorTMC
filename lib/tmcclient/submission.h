#ifndef SUBMISSION_H
#define SUBMISSION_H

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

    enum class Status
    {
        Ok,
        Fail,
        Error,
        Processing,
        Hidden
    };

    void setStatus(Status status);
    void setPoints(QList<QString> points);
    void setPassed(bool passed);
    void setSubmissionTime(QDate time);
    void setError(QString error);
    void setSolutionUrl(QUrl url);

    int getId() const;
    Status getStatus() const;
    QList<QString> getPoints() const;
    bool allTestPassed() const;
    QDate submissionTime() const;
    QString errorString() const;
    QUrl solutionUrl() const;

    static Submission fromJson(const int id, const QJsonObject jsonSubmission);

private:

    int m_id;
    Status m_status;
    QList<QString> m_points;
    bool m_testsPassed;
    QDate m_submissionTime;
    QString m_error;
    QUrl m_solutionUrl;

};

#endif // SUBMISSION_H
