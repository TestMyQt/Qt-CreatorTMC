#include "course.h"
#include "exercise.h"

Course::Course()
{

}

bool Course::operator==(const Course &other) const
{
    return other.getId() == m_id &&
            other.getName() == m_name;
}

bool Course::operator!=(const Course &other) const
{
    return !(*this == other);
}

QMap<int, Exercise> Course::getExercises()
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

void Course::setUpdates(bool updates)
{
    m_updates = updates;
}

int Course::getId() const
{
    return m_id;
}

QString Course::getName() const
{
    return m_name;
}

Exercise Course::getExercise(const int id)
{
    return m_exercises.value(id, Exercise());
}

Exercise Course::getExercise(const Exercise ex)
{
    return m_exercises.value(ex.getId(), Exercise());
}

void Course::addExercise(const Exercise e)
{
    m_exercises.insert(e.getId(), e);
}

bool Course::hasUpdates()
{
    return m_updates;
}

Course Course::fromJson(const QJsonObject jsonCourse)
{
    Course course;
    course.setName(jsonCourse["name"].toString());
    course.setId(jsonCourse["id"].toInt());
    return course;
}

Course Course::fromQSettings(QSettings *settings)
{
    Course course;
    course.setName(settings->value("courseName", "").toString());
    course.setId(settings->value("courseId", -1).toInt());
    return course;
}

void Course::toQSettings(QSettings *settings, Course c)
{
    settings->setValue("courseName", c.getName());
    settings->setValue("courseId", c.getId());
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
