#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    this->id = id;
    this->name = name;
}

int Exercise::getId() const {
    return this->id;
}

QString Exercise::getName() const {
    return this->name;
}

QString Exercise::getLocation() const {
    return this->location;
}

QString Exercise::getChecksum() {
    return this->checksum;
}

void Exercise::setId(int id) {
    this->id = id;
}

void Exercise::setName(QString name) {
    this->name = name;
}

void Exercise::setLocation(QString location) {
    this->location = location;
}

void Exercise::setChecksum(QString checksum) {
    this->checksum = checksum;
}
