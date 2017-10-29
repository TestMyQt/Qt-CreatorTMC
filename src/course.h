#ifndef COURSE_H
#define COURSE_H

#include <QList>
#include "exercise.h"

class Course
{
public:
    Course();
    void setId(int id);
    void setTitle(QString title);
    int getId();
    QString getTitle() const;
    Exercise getExercise(int id);
    void addExercise(Exercise e);
    QList<Exercise> * getExercises();
private:
    int id;
    QString title;
    QList<Exercise> exercises;
};

#endif // COURSE_H
