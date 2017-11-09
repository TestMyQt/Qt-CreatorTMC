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

void Course::setTitle(QString title)
{
    m_title = title;
}

int Course::getId() {
    return m_id;
}

QString Course::getTitle() const
{
    return m_title;
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

void Course::saveSettings()
{
    QSettings settings("TestMyQt", "Exercises");
    settings.beginGroup(m_title);
        settings.setValue("id", m_id);
    settings.endGroup();
    settings.deleteLater();
}

void Course::loadSettings(QString title)
{
    QSettings settings("TestMyQt", "Exercises");
    settings.beginGroup(title);
    setId(settings.value("id").toInt());
        QStringList exerciseList = settings.childGroups();
        foreach (QString exercise, exerciseList) {
            settings.beginGroup(exercise);
                Exercise ex(settings.value("id").toInt(), exercise);
                ex.setChecksum(settings.value("chekcsum", "").toString());
                ex.setLocation(settings.value("location", "").toString());
                ex.setDlDate(settings.value("dlDate", "").toString());
                ex.setOpenStatus(settings.value("openStatus", 0).toBool());
            settings.endGroup();
            addExercise(ex);
        }
    settings.endGroup();
    settings.deleteLater();
}
