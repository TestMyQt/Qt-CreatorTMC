#include "submission.h"

#include <QJsonObject>
#include <QJsonArray>

Submission::Submission(int id) :
    m_id(id)
{
    m_status = Status::Error;
}

bool Submission::operator==(const Submission &other) const
{
    return other.getId() == m_id &&
            other.getStatus() == m_status;
}

bool Submission::operator!=(const Submission &other) const
{
    return !(*this == other);
}

int Submission::getId() const
{
    return m_id;
}

Submission::Status Submission::getStatus() const
{
    return m_status;
}

QList<QString> Submission::getPoints() const
{
    return m_points;
}

QList<QString> Submission::getTestCases() const
{
    return m_testCases;
}

QString Submission::errorString() const
{
    return m_error;
}

void Submission::setStatus(Status status)
{
    m_status = status;
}

void Submission::setPoints(QList<QString> points)
{
    m_points = points;
}

void Submission::setTestCases(QList<QString> cases)
{
    m_testCases = cases;
}

void Submission::setPassed(bool passed)
{
    m_testsPassed = passed;
}

void Submission::setSubmissionTime(QDate time)
{
    m_submissionTime = time;
}

void Submission::setError(QString error)
{
    m_error = error;
}

void Submission::setSolutionUrl(QUrl url)
{
    m_solutionUrl = url;
}

Submission Submission::fromJson(const int id, const QJsonObject jsonSubmission)
{
    Submission submission(id);
    QString jsonStatus = jsonSubmission["status"].toString();
    if (jsonStatus == "ok") {
        submission.setStatus(Status::Ok);
        QStringList testCases;
        foreach (QString testCase, testCases) {
            testCases << testCase;
        }
        submission.setTestCases(testCases);
    }

    if (jsonStatus == "fail") {
        submission.setStatus(Status::Fail);
    }

    if (jsonStatus == "error") {
        submission.setStatus(Status::Error);
    }

    if (jsonStatus == "processing") {
        submission.setStatus(Status::Processing);
    }

    if (jsonStatus == "hidden") {
        submission.setStatus(Status::Hidden);
    }

    submission.setPassed(jsonSubmission["all_tests_passed"].toBool());
    QJsonArray jsonPoints = jsonSubmission["points"].toArray();
    QList<QString> points;
    foreach (QJsonValue point, jsonPoints) {
        points << point.toString();
    }
    submission.setPoints(points);
    submission.setSubmissionTime(QDate::fromString(jsonSubmission["submitted_at"].toString()));
    submission.setError(jsonSubmission["error"].toString());

    return submission;
}
