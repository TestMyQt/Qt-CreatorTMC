#include "course.h"

Course::Course()
{

}

QList<Exercise> * Course::getExercises()
{
    return &this->exercises;
}

void Course::setId(int id) {
    this->id = id;
}

void Course::setTitle(QString title) {
    this->title = title;
}

int Course::getId() {
    return id;
}

QString Course::getTitle() const {
    return title;
}

Exercise Course::getExercise(int id) {
    foreach (Exercise e, exercises) {
        if (e.getId() == id) {
            return e;
        }
    }
    return Exercise(-1, QString("null"));
}

void Course::addExercise(Exercise e) {
    exercises.append(e);
}
