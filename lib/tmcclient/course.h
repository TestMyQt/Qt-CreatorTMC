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
    void saveSettings();
    static Course fromJson(QJsonObject jsonCourse);
    static Course fromQSettings(QSettings *settings);
    static void toQSettings(QSettings *settings, Course c);
    void exerciseListFromQSettings(QSettings *settings);
private:
    int m_id;
    QString m_name;
    QList<Exercise> m_exercises;
};

Q_DECLARE_METATYPE(Course)

#endif // COURSE_H
