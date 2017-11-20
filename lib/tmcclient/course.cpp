/*!
    \class Course
    \inmodule lib/tmcclient
    \inheaderfile tmcclient.h
    \brief Class \l Course is a simple representation of a TMC course that includes
    the course's exercises as a collection of \l Exercise objects.
*/

#include "course.h"

Course::Course()
{

}

/*! Returns a pointer to the collection of the \l Exercise objects that
represent the whole set of the course's exercises. */
QList<Exercise> * Course::getExercises()
{
    return &m_exercises;
}

/*! Sets the course ID. */
void Course::setId(int id) {
    m_id = id;
}

/*! Sets the course title. */
void Course::setTitle(QString title) {
    m_title = title;
}

/*! Getter function for the course ID. */
int Course::getId() {
    return m_id;
}

/*! Getter function for the course title. */
QString Course::getTitle() const {
    return m_title;
}

/*! Returns the Exercise object specified by \a id. If no such Exercise
object is found, a "blank" Exercise object is returned (rather than null). */
Exercise Course::getExercise(int id) {
    foreach (Exercise e, m_exercises) {
        if (e.getId() == id) {
            return e;
        }
    }
    return Exercise(-1, QString("null"));
}

void Course::addExercise(Exercise e) {
    m_exercises.append(e);
}
