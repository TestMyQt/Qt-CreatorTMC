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

    void setId(int id);
    void setName(QString name);
    void setUpdates(bool updates);
    int getId() const;
    QString getName() const;
    Exercise getExercise(int id);
    Exercise getExercise(const Exercise ex);
    void addExercise(const Exercise e);
    QMap<int, Exercise> getExercises();
    bool hasUpdates();
    void saveSettings();
    static Course fromJson(const QJsonObject jsonCourse);
    static Course fromQSettings(QSettings *settings);
    static void toQSettings(QSettings *settings, Course c);
    void exerciseListFromQSettings(QSettings *settings);
private:
    int m_id;
    QString m_name;
    bool m_updates;
    QMap<int, Exercise> m_exercises;
};

Q_DECLARE_METATYPE(Course)

#endif // COURSE_H
