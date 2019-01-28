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
    Exercise(int id,const QString &name);

    enum State {
        None,
        Open,
        Downloaded
    };

    bool operator==(const Exercise &other) const;
    bool operator!=(const Exercise &other) const;
    bool operator!() const;
    bool operator<(const Exercise &other) const;

    void setId(int id);
    void setName(const QString &name);
    void setLocation(const QString &location);
    void setChecksum(const QString &checksum);
    void setDeadline(const QString &date);
    void setState(State state);
    void setDownloaded(bool downloaded);
    void setUnzipped(bool zipped);
    void addSubmission(Submission &sub);

    int getId() const;
    QString getName() const;
    QString getProFile() const;
    QString getLocation() const;
    QString getChecksum() const;
    QDateTime getDeadline() const;
    Exercise::State getState() const;
    bool isDownloaded() const;
    bool isUnzipped() const;
    QMap<int, Submission> getSubmissions() const;

    void saveQSettings(QSettings &settings, const QString &courseName);
    static Exercise fromQSettings(QSettings &settings, const QString &exerciseName);
    static Exercise fromJson(const QJsonObject &jsonExercise);

private:
    QMap<int, Submission> m_submissions;
    QString m_name;
    QString m_location;
    QString m_checksum;
    QDateTime m_deadline;
    int m_id;
    State m_state;
    bool m_downloaded;
    bool m_unzipped;

};

Q_DECLARE_METATYPE(Exercise)

#endif // EXERCISE_H
