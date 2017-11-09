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
    void saveSettings();
    void loadSettings(QString title);
private:
    int m_id;
    QString m_title;
    QList<Exercise> m_exercises;
};

#endif // COURSE_H
