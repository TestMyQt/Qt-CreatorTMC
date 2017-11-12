#include "course.h"

Course::Course()
{

}

QList<Exercise> * Course::getExercises()
{
    return &m_exercises;
}

void Course::setId(int id) {
    m_id = id;
}

void Course::setTitle(QString title) {
    m_title = title;
}

int Course::getId() {
    return m_id;
}

QString Course::getTitle() const {
    return m_title;
}

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
