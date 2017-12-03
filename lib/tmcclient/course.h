#ifndef COURSE_H
#define COURSE_H

#include <QList>
#include <QSettings>
#include <QJsonObject>
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
    static Course fromJson(QJsonObject jsonCourse);
    static Course *fromQSettings(QSettings *settings);
    void exerciseListFromQSettings(QSettings *settings);
private:
    int m_id;
    QString m_name;
    QList<Exercise> m_exercises;
};

#endif // COURSE_H
