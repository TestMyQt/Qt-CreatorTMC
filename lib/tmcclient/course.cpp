#include "course.h"
#include "exercise.h"

#include <QSettings>

Course::Course()
{

}

QList<Exercise> * Course::getExercises()
{
    return &m_exercises;
}

void Course::setId(int id)
{
    m_id = id;
}

void Course::setName(QString name)
{
    m_name = name;
}

int Course::getId() const
{
    return m_id;
}

QString Course::getName() const
{
    return m_name;
}

Exercise Course::getExercise(int id)
{
    foreach (Exercise e, m_exercises) {
        if (e.getId() == id) {
            return e;
        }
    }
    return Exercise(-1, QString("null"));
}

void Course::addExercise(Exercise e)
{
    m_exercises.append(e);
}

void Course::loadSettings()
{
    QSettings settings("TestMyQt", "Exercises");
    settings.beginGroup(m_name);
        QStringList exerciseList = settings.childGroups();
        foreach (QString exercise, exerciseList) {
            settings.beginGroup(exercise);
                Exercise ex(settings.value("id").toInt(), exercise);
                ex.setChecksum(settings.value("chekcsum", "").toString());
                ex.setLocation(settings.value("location", "").toString());
                ex.setDlDate(settings.value("dlDate", "").toString());
                ex.setOpenStatus(settings.value("openStatus", false).toBool());
            settings.endGroup();
            addExercise(ex);
        }
    settings.endGroup();
    settings.deleteLater();
}
