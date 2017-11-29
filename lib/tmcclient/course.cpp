/*!
    \class Course
    \inmodule lib/tmcclient
    \inheaderfile course.h
    \brief Class \l Course is a simple representation of a TMC course that includes
    the course's exercises as a collection of \l Exercise objects.
*/

#include "course.h"
#include "exercise.h"

Course::Course()
{

}

/*!
    Returns a pointer to the \l Course object's private \l Exercise
    collection.
*/
QList<Exercise> Course::getExercises()
{
    return m_exercises;
}

/*! Sets the course ID to \a id. */
void Course::setId(int id)
{
    m_id = id;
}

/*! Sets the course name to \a name. */
void Course::setName(QString name)
{
    m_name = name;
}

/*! Getter function for the course ID. */
int Course::getId() const
{
    return m_id;
}

/*! Getter function for the course name. */
QString Course::getName() const
{
    return m_name;
}

/*!
    Returns the \l Exercise object specified by \a id. If no such \l Exercise
    object is found, a "blank" \l Exercise object is returned (rather than null).
*/
Exercise Course::getExercise(int id)
{
    foreach (Exercise e, m_exercises) {
        if (e.getId() == id) {
            return e;
        }
    }
    return Exercise(-1, QString("null"));
}

/*! Adds parameter \a e to the \l Course object's private \l Exercise collection. */
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
