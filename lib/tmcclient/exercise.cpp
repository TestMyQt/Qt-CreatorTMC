/*!
    \class Exercise
    \inmodule lib/tmcclient
    \inheaderfile exercise.h
    \brief Class \l Exercise objects represent TMC course exercises.
*/

#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    m_id = id;
    m_name = name;
}

/*! Getter function for the exercise ID. */
int Exercise::getId() const {
    return m_id;
}

/*! Getter function for the exercise name. */
QString Exercise::getName() const {
    return m_name;
}

/*! Getter function for the directory location of the exercise. */
QString Exercise::getLocation() const {
    return m_location;
}

QString Exercise::getChecksum() const {
    return m_checksum;
}

/*! Sets the exercise ID to \a id. */
void Exercise::setId(int id) {
    m_id = id;
}

/*! Sets the exercise name to \a name. */
void Exercise::setName(QString name) {
    m_name = name;
}

/*! Sets the directory location of the exercise to \a location. */
void Exercise::setLocation(QString location) {
    m_location = location;
}

void Exercise::setChecksum(QString checksum) {
    m_checksum = checksum;
}
