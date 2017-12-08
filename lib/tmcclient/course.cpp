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

bool Course::operator==(const Course &other) const
{
    return other.getId() == m_id &&
            other.getName() == m_name;
}

bool Course::operator!=(const Course &other) const
{
    return !(*this == other);
}

/*!
    Returns a pointer to the \l Course object's private \l Exercise
    collection.
*/
QMap<int, Exercise> Course::getExercises()
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
Exercise Course::getExercise(const int id)
{
    return m_exercises.value(id, Exercise());
}

Exercise Course::getExercise(const Exercise ex)
{
    return m_exercises.value(ex.getId(), Exercise());
}

/*! Adds parameter \a e to the \l Course object's private \l Exercise collection. */
void Course::addExercise(const Exercise ex)
{
    m_exercises.insert(ex.getId(), ex);
}

bool Course::hasExercise(Exercise ex)
{
    return m_exercises.contains(ex.getId());
}

/*!
    Uses the \l {http://doc.qt.io/qt-5/qjsonobject.html} {QJsonObject}
    parameter \a jsonCourse to initialize a new \l Course object which
    is the return value.
 */
Course Course::fromJson(const QJsonObject jsonCourse)
{
    Course course;
    course.setName(jsonCourse["name"].toString());
    course.setId(jsonCourse["id"].toInt());
    return course;
}

/*!
    Creates a new \l Course object from the \l {http://doc.qt.io/qt-5/qsettings.html}
    {QSettings} pointer parameter \a settings and returns it. Creating the \l Course
    object includes setting the course name and ID to the values specified in \a
    settings. If \a settings doesn't contain the appropriate values, defaults are used.
 */
Course Course::fromQSettings(QSettings *settings)
{
    Course course;
    course.setName(settings->value("courseName", "").toString());
    course.setId(settings->value("courseId", -1).toInt());
    return course;
}

/*!
    Saves the \l {Course::getName()} {name} and \l {Course::getId()} {ID} fields of
    the \l Course parameter \a c into the \l {http://doc.qt.io/qt-5/qsettings.html}
    {QSettings} object pointed to by parameter \a settings.
*/
void Course::toQSettings(QSettings *settings, Course c)
{
    settings->setValue("courseName", c.getName());
    settings->setValue("courseId", c.getId());
}

/*!
    Uses the \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings} pointer
    \a settings to set the \l Exercise list of the \l Course object.

    \note If the \l Course object already contains items in its \l Exercise
    list, they are preserved. Any new exercises from \a settings are added
    to the existing ones.
*/
void Course::exerciseListFromQSettings(QSettings *settings)
{
    settings->beginGroup(m_name);
        QStringList exerciseList = settings->childGroups();
        foreach (QString exercise, exerciseList) {
            settings->beginGroup(exercise);
                Exercise ex = Exercise::fromQSettings(settings);
            settings->endGroup();
            addExercise(ex);
        }
    settings->endGroup();
}
