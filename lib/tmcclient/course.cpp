/*!
    \class Course
    \inmodule lib/tmcclient
    \inheaderfile course.h
    \brief Class \l Course is a simple representation of TMC courses that includes
        the course's exercises as values in a \l {http://doc.qt.io/qt-5/qmap.html}
        {QMap} object.

    The keys are the IDs of the \l Exercise objects; each exercise ID is mapped to
    the corresponding \l Exercise object.
*/

#include "course.h"

Course::Course()
{
    m_id = -1;
    m_name = "";
}

/*!
    Overloading the \c{==} operator. The \l Course object is compared with the
    parameter \a other. Two \l Course objects are considered equal if both their
    \l {Course::getName()} {name} and \l {Course::getId()} {id} fields are equal.
    Returns \c true if the two \l Course objects are equal and \c false otherwise.
*/
bool Course::operator==(const Course &other) const
{
    return other.getId() == m_id &&
            other.getName() == m_name;
}

/*!
    Overloading the \c{!=} operator. The \l Course object is compared with the
    parameter \a other. Two \l Course objects are considered distinct if either
    their \l {Course::getName()} {name} or \l {Course::getId()} {id} fields differ.
    Returns \c true if the two \l Course objects are distinct and \c false otherwise.
*/
bool Course::operator!=(const Course &other) const
{
    return !(*this == other);
}

bool Course::operator!() const
{
    return m_id == -1;
}

/*!
    Getter function for the \l Course object's collection of \l Exercise
    ID-to-object mappings.
*/
QMap<int, Exercise> Course::getExercises()
{
    return m_exercises;
}

/*!
    Sets the course ID to \a id.
*/
void Course::setId(int id)
{
    m_id = id;
}

/*!
    Sets the course name to \a name.
*/
void Course::setName(QString name)
{
    m_name = name;
}

/*!
    Getter function for the course ID.
*/
int Course::getId() const
{
    return m_id;
}

/*!
    Getter function for the course name.
*/
QString Course::getName() const
{
    return m_name;
}

/*!
    Returns the \l Exercise object specified by \a id from the \l Course object's
    collection. If no \l Exercise object with an ID equal to \a id is found, the
    function returns a new \l Exercise object instantiated by \l Exercise::Exercise().
*/
Exercise Course::getExercise(const int id)
{
    return m_exercises.value(id, Exercise());
}

/*!
    Returns the \l Exercise object with the same ID as that of the parameter \a ex.
    If no such \l Exercise object is found in the \l Course object's collection,
    the function returns a new \l Exercise object instantiated by \l Exercise::Exercise().
*/
Exercise Course::getExercise(const Exercise ex)
{
    return m_exercises.value(ex.getId(), Exercise());
}

/*!
    Adds parameter \a ex to the \l Course object's \l Exercise collection.
*/
void Course::addExercise(const Exercise ex)
{
    m_exercises.insert(ex.getId(), ex);
}

/*!
    Returns \c true if the \l Course object's collection contains an \l Exercise
    object with the same ID as that of parameter \a ex; otherwise returns \c false.
*/
bool Course::hasExercise(Exercise ex)
{
    return m_exercises.contains(ex.getId());
}

/*!
    Uses the \l {http://doc.qt.io/qt-5/qjsonobject.html} {QJsonObject}
    parameter \a jsonCourse to initialize and return a new \l Course object.
    The ID and name fields of the \l Course object are set to the values
    extracted from \a jsonCourse.
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
