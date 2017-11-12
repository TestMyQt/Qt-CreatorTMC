#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    m_id = id;
    m_name = name;
}

int Exercise::getId() const {
    return m_id;
}

QString Exercise::getName() const {
    return m_name;
}

QString Exercise::getLocation() const {
    return m_location;
}

QString Exercise::getChecksum() const {
    return m_checksum;
}

void Exercise::setId(int id) {
    m_id = id;
}

void Exercise::setName(QString name) {
    m_name = name;
}

void Exercise::setLocation(QString location) {
    m_location = location;
}

void Exercise::setChecksum(QString checksum) {
    m_checksum = checksum;
}
