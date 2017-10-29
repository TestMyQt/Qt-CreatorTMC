#ifndef EXERCISE_H
#define EXERCISE_H

#include <QString>

class Exercise
{
public:
    Exercise(int id, QString name);
    void setId(int id);
    void setName(QString name);
    int getId();
    QString getName() const;

private:
    int id;
    QString name;
};

#endif // EXERCISE_H
