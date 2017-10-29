#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    this->id = id;
    this->name = name;
}

int Exercise::getId() {
    return this->id;
}

QString Exercise::getName() const{
    return this->name;
}

void Exercise::setId(int id) {
    this->id = id;
}

void Exercise::setName(QString name) {
    this->name = name;
}

