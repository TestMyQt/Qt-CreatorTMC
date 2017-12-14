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
    ~Exercise() { }

    bool operator==(const Exercise &other) const;
    bool operator!=(const Exercise &other) const;
    bool operator!() const;

    void setId(int id);
    void setName(QString name);
    void setLocation(QString location);
    void setChecksum(QString checksum);
    void setDlDate(QString dlDate);
    void setOpenStatus(bool openStatus);
    void setDownloaded(bool downloaded);
    void setUnzipped(bool zipped);
    void addSubmission(Submission sub);

    int getId() const;
    QString getName() const;
    QString getLocation() const;
    QString getChecksum() const;
    QString getDlDate() const;
    bool getOpenStatus() const;
    bool isDownloaded() const;
    bool isUnzipped() const;
    QMap<int, Submission> getSubmissions() const;

    void saveQSettings(QSettings *settings, const QString courseName);
    static Exercise fromQSettings(QSettings *settings, QString exerciseName);
    static Exercise fromJson(const QJsonObject jsonExercise);

private:
    int m_id;
    QString m_name;
    QString m_location;
    QString m_checksum;
    QString m_dlDate;
    bool m_downloaded;
    bool m_unzipped;
    bool m_openStatus;
    QMap<int, Submission> m_submissions;

};

Q_DECLARE_METATYPE(Exercise)

#endif // EXERCISE_H
