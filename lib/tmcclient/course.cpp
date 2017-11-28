#include "course.h"
#include "exercise.h"

Course::Course()
{

}

QList<Exercise> Course::getExercises()
{
    return m_exercises;
}

void Course::setId(int id)
{
    m_id = id;
}

void Course::setName(QString name)
{
    m_name = name;
}

int Course::getId() const
{
    return m_id;
}

QString Course::getName() const
{
    return m_name;
}

Exercise Course::getExercise(int id)
{
    foreach (Exercise e, m_exercises) {
        if (e.getId() == id) {
            return e;
        }
    }
    return Exercise(-1, QString("null"));
}

void Course::addExercise(Exercise e)
{
    m_exercises.append(e);
}

Course Course::fromJson(QJsonObject jsonCourse)
{
    Course course;
    course.setName(jsonCourse["name"].toString());
    course.setId(jsonCourse["id"].toInt());
    return course;
}

Course* Course::fromQSettings(QSettings *settings)
{
    Course *course = new Course;
    course->setName(settings->value("courseName", "").toString());
    course->setId(settings->value("courseId", -1).toInt());
    return course;
}

void Course::exerciseListFromQSettings(QSettings *settings)
{
    settings->beginGroup("Exercises");
    settings->beginGroup(m_name);
        QStringList exerciseList = settings->childGroups();
        foreach (QString exercise, exerciseList) {
            settings->beginGroup(exercise);
                Exercise ex = Exercise::fromQSettings(settings);
            settings->endGroup();
            addExercise(ex);
        }
    settings->endGroup();
    settings->deleteLater();
}
