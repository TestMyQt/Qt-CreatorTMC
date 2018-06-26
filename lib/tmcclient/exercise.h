#ifndef EXERCISE_H
#define EXERCISE_H

#include "submission.h"

#include <QString>
#include <QSettings>
#include <QJsonObject>

class Exercise
{
public:
    Exercise();
    Exercise(int id, QString name);
    ~Exercise() = default;

    bool operator==(const Exercise &other) const;
    bool operator!=(const Exercise &other) const;
    bool operator!() const;

    void setId(int id);
    void setName(QString name);
    void setLocation(QString location);
    void setChecksum(QString checksum);
    void setDeadline(const QString &date);
    void setOpenStatus(bool openStatus);
    void setDownloaded(bool downloaded);
    void setUnzipped(bool zipped);
    void addSubmission(Submission &sub);

    int getId() const;
    QString getName() const;
    QString getProFile() const;
    QString getLocation() const;
    QString getChecksum() const;
    QDateTime getDeadline() const;
    bool getOpenStatus() const;
    bool isDownloaded() const;
    bool isUnzipped() const;
    QMap<int, Submission> getSubmissions() const;

    void saveQSettings(QSettings *settings, const QString courseName);
    static Exercise fromQSettings(QSettings *settings, QString exerciseName);
    static Exercise fromJson(const QJsonObject jsonExercise);

private:
    QMap<int, Submission> m_submissions;

    QString m_name;
    QString m_location;
    QString m_checksum;
    QDateTime m_deadline;
    int m_id;
    bool m_downloaded;
    bool m_unzipped;
    bool m_openStatus;

};

Q_DECLARE_METATYPE(Exercise)

#endif // EXERCISE_H
