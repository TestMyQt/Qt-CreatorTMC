#ifndef COURSE_H
#define COURSE_H

#include <QList>
#include "exercise.h"

class Course
{
public:
    Course();
    void setId(int id);
    void setName(QString name);
    int getId() const;
    QString getName() const;
    Exercise getExercise(int id);
    void addExercise(Exercise e);
    QList<Exercise> getExercises();
    void saveSettings();
    void loadSettings();
private:
    int m_id;
    QString m_name;
    QList<Exercise> m_exercises;
};

#endif // COURSE_H
