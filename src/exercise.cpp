#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    this->id = id;
    this->name = name;
}

Exercise::getId() {
    return this->id;
}

Exercise::getName() {
    return this->name;
}

Exercise::setId(int id) {
    this->id = id;
}

Exercise::setName(QString name) {
    this->name = name;
}

