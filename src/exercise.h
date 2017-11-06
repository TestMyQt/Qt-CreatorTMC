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
    QString getChecksum();

private:
    int id;
    QString name;
    QString location;
    QString checksum;
};

#endif // EXERCISE_H
