#include "exercise.h"

Exercise::Exercise()
{
    m_id = -1;
    m_name = "";
    m_downloaded = false;
    m_unzipped = false;
}

Exercise::Exercise(int id, QString name)
{
    m_id = id;
    m_name = name;
    m_downloaded = false;
    m_unzipped = false;
}

bool Exercise::operator==(const Exercise &other) const
{
    return other.getId() == m_id &&
            other.getChecksum() == m_checksum;
}

bool Exercise::operator!=(const Exercise &other) const
{
    return !(*this == other);
}

bool Exercise::operator!() const
{
    return m_id == -1;
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

bool Exercise::isDownloaded() const
{
    return m_downloaded;
}

bool Exercise::isUnzipped() const
{
    return m_unzipped;
}

void Exercise::addSubmission(Submission sub)
{
    m_submissions.insert(sub.getId(), sub);
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

void Exercise::setDownloaded(bool downloaded)
{
    m_downloaded = downloaded;
}

void Exercise::setUnzipped(bool zipped)
{
    m_unzipped = zipped;
}

void Exercise::saveQSettings(QSettings *settings, const QString courseName)
{
    // Yay or nay on the indentation?
    settings->beginGroup(courseName);
        settings->beginGroup(m_name);
            settings->setValue("id", m_id);
            settings->setValue("checksum", m_checksum);
            settings->setValue("location", m_location);
            settings->setValue("dlDate", m_dlDate);
            settings->setValue("openStatus", m_openStatus);
            settings->setValue("downloaded", m_downloaded);
            settings->setValue("unzipped", m_unzipped);
        settings->endGroup();
    settings->endGroup();
}

Exercise Exercise::fromQSettings(QSettings *settings)
{
    Exercise ex = Exercise(settings->value("id").toInt(),
                           settings->value("name").toString());

    ex.setChecksum(settings->value("checksum", "").toString());
    ex.setLocation(settings->value("location", "").toString());
    ex.setDlDate(settings->value("dlDate", "").toString());
    ex.setOpenStatus(settings->value("openStatus", false).toBool());
    ex.setDownloaded(settings->value("downloaded", false).toBool());
    ex.setUnzipped(settings->value("unzipped", false).toBool());
    return ex;
}

Exercise Exercise::fromJson(const QJsonObject jsonExercise)
{

    Exercise fromJson = Exercise(jsonExercise["id"].toInt(),
            jsonExercise["name"].toString());
    fromJson.setChecksum(jsonExercise["checksum"].toString());
    fromJson.setDlDate(jsonExercise["deadline"].toString());

    return fromJson;
}
