#ifndef COURSE_H
#define COURSE_H

#include <QMap>
#include <QSettings>
#include <QJsonObject>
#include "exercise.h"

class Course
{
public:
    Course();
    bool operator==(const Course &other) const;
    bool operator!=(const Course &other) const;
    bool operator!() const;

    void setId(int id);
    void setName(QString name);
    int getId() const;
    QString getName() const;
    Exercise getExercise(const int id);
    Exercise getExercise(const Exercise ex);
    void addExercise(const Exercise ex);
    QMap<int, Exercise> getExercises();
    bool hasExercise(Exercise ex);
    static Course fromJson(const QJsonObject jsonCourse);
    static Course fromQSettings(QSettings *settings);
    static void toQSettings(QSettings *settings, Course c);
    void exerciseListFromQSettings(QSettings *settings);
private:
    int m_id;
    QString m_name;
    QMap<int, Exercise> m_exercises;
};

Q_DECLARE_METATYPE(Course)
Q_DECLARE_METATYPE(Course*)

#endif // COURSE_H
