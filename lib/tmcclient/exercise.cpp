/*!
    \class Exercise
    \inmodule lib/tmcclient
    \inheaderfile exercise.h
    \brief \l Exercise objects represent TMC course exercises.

    In practice TMC exercises are directories that contain the files of a miniscule
    software project. One of the fields of class \l Exercise is one that specifies its
    location. The location is the parent directory of the exercise directory itself.
    The location should be named after the name of the course the exercise belongs to.
    For example, if the course name is Foo and the exercise is called Bar, then the
    exercise path should look something like \c {~/tmc/Foo/Bar}.
*/

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

/*!
    Returns \c true if the \l Exercise object is equal to the parameter \a other;
    otherwise returns \c false. The overloaded operator expression \c {e1 == e2}
    evaluates to \c true when both the \l {Exercise::getId()} {id} and
    \l {Exercise::getChecksum()} {checksum} fields of the \l Exercise objects
    \c e1 and \c e2 are equal; otherwise the expression evaluates to \c false.
*/
bool Exercise::operator==(const Exercise &other) const
{
    return other.getId() == m_id &&
            other.getChecksum() == m_checksum;
}

/*!
    Returns \c true if the \l Exercise object is distinct from parameter \a other;
    otherwise returns \c false. The overloaded operator expression \c {e1 != e2}
    evaluates to \c true when either the \l {Exercise::getId()} {id} or
    \l {Exercise::getChecksum()} {checksum} fields of the operands are not equal;
    otherwise the expression evaluates to \c false.
*/
bool Exercise::operator!=(const Exercise &other) const
{
    return !(*this == other);
}

/*!
    The overloaded \b {!} operator is used to check whether the \l Exercise
    object is in an uninitialized state. If this is the case, the return value
    is \c true, otherwise it is \c false.

    \note The telltale sign of an uninitialized \l Exercise object is an
    \l {Exercise::getId()} {id} field set to the value -1.
*/
bool Exercise::operator!() const
{
    return m_id == -1;
}

bool Exercise::operator<(const Exercise &other) const
{
    return m_id < other.getId();
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

// Part1-Exercise1 -> Part1/Exercise1

/*!
    Gets the location of the .pro file for the \l Exercise object.
*/
QString Exercise::getProFile() const
{
    QStringList parts = m_name.split(QStringLiteral("-"));
    return QStringLiteral("%1/%2/%3.pro").arg(m_location, parts.join(QStringLiteral("/")), parts.last());
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
*/
QString Exercise::getChecksum() const
{
    return m_checksum;
}

/*!
    Getter function for the deadline date field of the \l Exercise object.
*/
QDateTime Exercise::getDeadline() const
{
    return m_deadline;
}

/*!
    Getter function for the open status of the \l Exercise object.
    If the corresponding TMC exercise project has been opened in Qt Creator,
    \c true is returned and \c false otherwise.
*/
bool Exercise::getOpenStatus() const
{
    return m_openStatus;
}

/*!
    Returns \c true if the TMC exercise corresponding to the \l Exercise object
    has been downloaded; otherwise returns false.

    \note Before downloading an actual TMC exercise (which comes in the form of
    a compressed archive) a list of available TMC exercises is downloaded. The
    exercises to be downloaded are selected from this list and are marked as
    downloaded only after a successful download operation of the exercise
    archive files.
*/
bool Exercise::isDownloaded() const
{
    return m_downloaded;
}

/*!
    Returns \c true if the archive file corresponding to the \l Exercise object
    has already been extracted; otherwise returns \c false.
*/
bool Exercise::isUnzipped() const
{
    return m_unzipped;
}

QMap<int, Submission> Exercise::getSubmissions() const
{
    return m_submissions;
}

void Exercise::addSubmission(Submission &sub)
{
    m_submissions.insert(sub.getId(), sub);
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
void Exercise::setDeadline(const QString &deadline)
{
    m_deadline = QDateTime::fromString(deadline, Qt::ISODateWithMs);
}

/*!
    Sets the open status of the \l Exercise object to \a openStatus. The open
    status should be set to \c true when the corresponding TMC exercise project
    is opened in Qt Creator; when the project is closed, the open status should
    be set to \c false.
*/
void Exercise::setOpenStatus(bool openStatus)
{
    m_openStatus = openStatus;
}

/*!
    Sets the download status of the \l Exercise object to the value of parameter
    \a downloaded.
*/
void Exercise::setDownloaded(bool downloaded)
{
    m_downloaded = downloaded;
}

/*!
    After being downloaded a TMC exercise is in memory in the form
    of a compressed (zip) archive. The obvious next step is to extract the
    archive and save the resulting directory in the local file system. After
    the extraction process has been completed \c {setUnzipped()} of the
    corresponding \l Exercise object is called with the parameter \a zipped
    set to \c true.
*/
void Exercise::setUnzipped(bool zipped)
{
    m_unzipped = zipped;
}

/*!
    Saves the \l Exercise object's state in persistent storage using the
    \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings} parameter \a settings.
    The \l {http://doc.qt.io/qt-5/qstring.html} {QString} parameter \a courseName
    is used as a prefix in forming the key names with \l
    {http://doc.qt.io/qt-5/qsettings.html#beginGroup} {QSettings::beginGroup()}.
*/
void Exercise::saveQSettings(QSettings *settings, const QString courseName)
{
    settings->beginGroup(courseName);
        settings->beginGroup(m_name);
            settings->setValue("id", m_id);
            settings->setValue("checksum", m_checksum);
            settings->setValue("location", m_location);
            settings->setValue("deadline", m_deadline);
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
    parameter in the calling function using
    \l {http://doc.qt.io/qt-5/qsettings.html#beginGroup} {QSettings::beginGroup()}.
*/
Exercise Exercise::fromQSettings(QSettings *settings, QString exerciseName)
{
    Exercise ex = Exercise(settings->value("id").toInt(), exerciseName);
    ex.setChecksum(settings->value("checksum").toString());
    ex.setLocation(settings->value("location").toString());
    ex.setDeadline(settings->value("deadline").toString());
    ex.setOpenStatus(settings->value("openStatus", false).toBool());
    ex.setDownloaded(settings->value("downloaded", false).toBool());
    ex.setUnzipped(settings->value("unzipped", false).toBool());
    return ex;
}

/*!
    Uses the \l {http://doc.qt.io/qt-5/qjsonobject.html} {QJsonObject}
    parameter \a jsonExercise to initialize and return a new \l Exercise object.
    Initializing the \l Exercise object involves setting the \l {Exercise::getName()}
    {name}, \l {Exercise::getId()} {id}, \l {Exercise::getChecksum()} {checksum}
    and \l {Exercise::getDlDate()} {deadline} fields to the values extracted from
    \a jsonExercise.
*/
Exercise Exercise::fromJson(const QJsonObject jsonExercise)
{
    Exercise fromJson = Exercise(jsonExercise["id"].toInt(),
            jsonExercise["name"].toString());
    fromJson.setChecksum(jsonExercise["checksum"].toString());
    fromJson.setDeadline(jsonExercise["deadline"].toString());

    return fromJson;
}
