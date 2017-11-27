#include "exercise.h"

#include <QSettings>

Exercise::Exercise(int id, QString name)
{
    m_id = id;
    m_name = name;
}

int Exercise::getId() const
{
    return m_id;
}

QString Exercise::getName() const
{
    return m_name;
}

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

void Exercise::setId(int id)
{
    m_id = id;
}

void Exercise::setName(QString name)
{
    m_name = name;
}

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
