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

#include <QJsonObject>

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

/*!
    Getter function for the exercise ID.
*/
int Exercise::getId() const
{
    return m_id;
}

/*!
    Getter function for the name field of the \l Exercise object.
*/
QString Exercise::getName() const
{
    return m_name;
}

/*!
    Getter function for the directory location of the exercise.
*/
QString Exercise::getLocation() const
{
    return m_location;
}

/*!
    Getter function for the checksum field of the \l Exercise object.

    \note See \l {Exercise::} {setChecksum()} for a short description of
    TMC exercise checksums.
*/
QString Exercise::getChecksum() const
{
    return m_checksum;
}

/*!
    Getter function for the deadline date field of the \l Exercise object.
*/
QString Exercise::getDlDate() const
{
    return m_dlDate;
}

/*!
    Getter function for the open status of the \l Exercise object.
    If the corresponding TMC exercise project has been opened in Qt Creator,
    \c true is returned and \c false otherwise.

    \b{TODO:} Do fact-check!
*/
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

/*!
    Sets the exercise ID to \a id.
*/
void Exercise::setId(int id)
{
    m_id = id;
}

/*!
    Sets the exercise name to \a name.
*/
void Exercise::setName(QString name)
{
    m_name = name;
}

/*!
    Sets the directory location of the exercise to \a location.
*/
void Exercise::setLocation(QString location)
{
    m_location = location;
}

/*!
    Sets the TMC exercise checksum field of the \l Exercise object
    to \a checksum.

    \b{TODO:} Add a short description of TMC exercise checksums.
*/
void Exercise::setChecksum(QString checksum)
{
    m_checksum = checksum;
}

/*!
    Sets the deadline date field of the \l Exercise object to \a dlDate.

    TMC exercises often come with a deadline. After the deadline has passed,
    no points are usually given for completing the exercise.
*/
void Exercise::setDlDate(QString dlDate)
{
    m_dlDate = dlDate;
}

/*!
    Sets the open status of the \l Exercise object to \a openStatus. The open
    status should be set to \c true when the corresponding TMC exercise project
    is opened in Qt Creator; when the project is closed, the open status should
    be set to \c false.

    \b{TODO:} Do fact-check!
*/
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

/*!
    Saves the \l Exercise object's state in persistent storage using
    \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings}. The \l
    {http://doc.qt.io/qt-5/qstring.html} {QString} parameter \a courseName
    is used as a prefix in forming the key names with \l
    {http://doc.qt.io/qt-5/qsettings.html#beginGroup} {QSettings::beginGroup()}.
*/
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

/*!
    Creates and returns an \l Exercise object based on the values in the \l
    {http://doc.qt.io/qt-5/qsettings.html} {QSettings} parameter \a settings.
    At least the values of the name and ID fields of the \l Exercise object
    should be present in \a settings.

    \note It is assumed that the appropriate prefix has been set for the \a settings
    parameter in the calling function using the \l
    {http://doc.qt.io/qt-5/qsettings.html#beginGroup} {beginGroup()} function.
 */
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
