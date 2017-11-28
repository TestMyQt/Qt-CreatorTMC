/*!
    \class Exercise
    \inmodule lib/tmcclient
    \inheaderfile exercise.h
    \brief \l Exercise objects represent TMC course exercises.

    In practice TMC exercises are directories that contain the files of a miniscule
    software project. One of the instance variables of \l Exercise is one that
    specifies its location. The location is the parent directory of the exercise
    directory itself.
*/

#include "exercise.h"

Exercise::Exercise(int id, QString name)
{
    m_id = id;
    m_name = name;
}

/*! Getter function for the exercise ID. */
int Exercise::getId() const
{
    return m_id;
}

/*! Getter function for the exercise name. */
QString Exercise::getName() const
{
    return m_name;
}

/*! Getter function for the directory location of the exercise. */
QString Exercise::getLocation() const
{
    return m_location;
}

QString Exercise::getChecksum() const
{
    return m_checksum;
}

QString Exercise::getDlDate() const
{
    return m_dlDate;
}

bool Exercise::getOpenStatus() const
{
    return m_openStatus;
}

/*! Sets the exercise ID to \a id. */
void Exercise::setId(int id)
{
    m_id = id;
}

/*! Sets the exercise name to \a name. */
void Exercise::setName(QString name)
{
    m_name = name;
}

/*! Sets the directory location of the exercise to \a location. */
void Exercise::setLocation(QString location)
{
    m_location = location;
}

void Exercise::setChecksum(QString checksum)
{
    m_checksum = checksum;
}

void Exercise::setDlDate(QString dlDate)
{
    m_dlDate = dlDate;
}

void Exercise::setOpenStatus(bool openStatus)
{
    m_openStatus = openStatus;
}

void Exercise::saveSettings(QString courseName)
{
    // Yay or nay on the indentation?
    QSettings settings("TestMyQt", "Exercises");
    settings.beginGroup(courseName);
        settings.beginGroup(m_name);
            settings.setValue("id", m_id);
            settings.setValue("checksum", m_checksum);
            settings.setValue("location", m_location);
            settings.setValue("dlDate", m_dlDate);
            settings.setValue("openStatus", m_openStatus);
        settings.endGroup();
    settings.endGroup();
    settings.deleteLater();
}

Exercise Exercise::fromQSettings(QSettings *settings)
{
    Exercise ex = Exercise(settings->value("id").toInt(),
                           settings->value("name").toString());

    ex.setChecksum(settings->value("checksum", "").toString());
    ex.setLocation(settings->value("location", "").toString());
    ex.setDlDate(settings->value("dlDate", "").toString());
    ex.setOpenStatus(settings->value("openStatus", false).toBool());
    return ex;
}
