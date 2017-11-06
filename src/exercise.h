#ifndef EXERCISE_H
#define EXERCISE_H

#include <QString>

class Exercise
{
public:
    Exercise(int id, QString name);
    void setId(int id);
    void setName(QString name);
    void setLocation(QString location);
    void setChecksum(QString checksum);
    int getId() const;
    QString getName() const;
    QString getLocation() const;
    QString getChecksum() const;

private:
    int m_id;
    QString m_name;
    QString m_location;
    QString m_checksum;
};

#endif // EXERCISE_H
