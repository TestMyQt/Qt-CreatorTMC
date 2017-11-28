#ifndef EXERCISE_H
#define EXERCISE_H

#include <QString>
#include <QSettings>

class Exercise
{
public:
    Exercise(int id, QString name);
    void setId(int id);
    void setName(QString name);
    void setLocation(QString location);
    void setChecksum(QString checksum);
    void setDlDate(QString dlDate);
    void setOpenStatus(bool openStatus);
    int getId() const;
    QString getName() const;
    QString getLocation() const;
    QString getChecksum() const;
    QString getDlDate() const;
    bool getOpenStatus() const;
    void saveSettings(QString courseName);
    static Exercise fromQSettings(QSettings *settings);

private:
    int m_id;
    QString m_name;
    QString m_location;
    QString m_checksum;
    QString m_dlDate;
    bool m_openStatus;
};

#endif // EXERCISE_H
