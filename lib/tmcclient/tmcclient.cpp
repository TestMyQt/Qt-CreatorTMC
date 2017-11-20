/*!
    \class TmcClient
    \inmodule lib/tmcclient
    \inheaderfile tmcclient.h
    \brief Class \l TmcClient is the primary means of communication with the TMC server.
*/

#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSettings>
#include <QBuffer>

#include <quazip/JlCompress.h>

#include "exercise.h"
#include "course.h"

TmcClient::TmcClient(QObject *parent) : QObject(parent)
{
}

/*!
    This is the function that gets called when the user clicks the \tt {Log in}
    button in the \tt {TMC Login} dialog. The values for the parameters \a username,
    \a password and \a savePassword are obtained from the \tt {TMC Login} dialog.
 */
void TmcClient::authenticate(QString username, QString password, bool savePassword)
{
    QString client_id = "8355b4a75a4191edfedeae7b074571278fd4987d4234c01569678b9ad11f526d";
    QString client_secret = "c2b1176a6189ceaa16cd51f805ef20ea6c993d36fdb76aa873ac35471d2df4f1";
    QString username_ = username;
    QString password_ = password;
    QString grant_type = "password";

    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", username);
    if (savePassword) {
        settings.setValue("password", password);
        settings.setValue("savePasswordChecked", "true");
    } else {
        settings.setValue("password", "");
        settings.setValue("savePasswordChecked", "false");
    }
    settings.deleteLater();

    QUrl url("https://tmc.mooc.fi/oauth/token");

    QUrlQuery params;
    params.addQueryItem("client_id", client_id);
    params.addQueryItem("client_secret", client_secret);
    params.addQueryItem("username", username_);
    params.addQueryItem("password", password_);
    params.addQueryItem("grant_type", grant_type);

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        authenticationFinished(reply);
    });
}

/*!
    The QtCreatorTMC plugin uses a single \l
    {http://doc.qt.io/qt-5/qnetworkaccessmanager.html} {QNetworkAccessManager}
    object for managing network communications. The object is initialized in
    \l TestMyCode::initialize(). \l {TmcClient::} {setNetworkManager()} is called
    from \l {TestMyCode::} {initialize()} with the address of the \c
    QNetworkAccessManager object as the value for parameter \a m.
 */
void TmcClient::setNetworkManager(QNetworkAccessManager *m)
{
    manager = m;
}

/*!
    //! A private function and as such it doesn't get any QDoc generated for it.
    Builds a \l {http://doc.qt.io/qt-5/qnetworkrequest.html} {QNetworkRequest}
    from the parameter \a url. The header of the \c QNetworkRequest object is
    set to include the access token provided by the TMC server earlier after
    successful authentication.
 */
QNetworkRequest TmcClient::buildRequest(QUrl url)
{
    QNetworkRequest request(url);
    QString a = "Bearer ";
    request.setRawHeader(QByteArray("Authorization") , QByteArray(a.append(accessToken).toUtf8()));
    return request;
}

QNetworkReply* TmcClient::doGet(QUrl url)
{
    QNetworkRequest request = buildRequest(url);
    QNetworkReply *reply = manager->get(request);
    return reply;
}

/*!
    Downloads the TMC exercise specified by the \l Exercise parameter \a ex.
    The exercise is delivered from the TMC server as zip archive data. Signal
    \l {http://doc.qt.io/qt-5/qnetworkreply.html} {QNetworkReply::finished} is
    connected to slot \l {TmcClient::} {exerciseZipReplyFinished()} which means
    that extraction of the files in the zip archive will begin automatically
    after the download completes.
 */
QNetworkReply* TmcClient::getExerciseZip(Exercise *ex)
{
    QUrl url(QString("https://tmc.mooc.fi/api/v8/core/exercises/%1/download").arg(ex->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseZipReplyFinished(reply, ex);
    });

    return reply;
}

/*!
    Creates a \l {http://doc.qt.io/qt-5/qnetworkreply.html} {QNetworkReply}
    object for downloading the exercise list associated with the \l Course
    parameter \a course and connects the object's signal
    \l {http://doc.qt.io/qt-5/qnetworkreply.html#finished}
    {QNetworkReply::finished} to slot \l {TmcClient::} {exerciseListReplyFinished()}.
 */
void TmcClient::getExerciseList(Course *course)
{
    QUrl url("https://tmc.mooc.fi/api/v8/core/courses/" + QString::number(course->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseListReplyFinished(reply, course);
    });
}

/*!
    Retrieves information about the currently logged in user from the TMC server.
    The information is delivered as a JSON document. The information includes...
    TODO: continue from here
 */
void TmcClient::getUserInfo()
{
    QUrl url("https://tmc.mooc.fi/api/v8/users/current");
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        qDebug() << reply->readAll();
        reply->deleteLater();
    });
}

/*!
    The primary purpose of the slot is to extract the access token
    from the parameter \a reply and save it in a member variable for
    future use. The slot is called when the \l
    {http://doc.qt.io/qt-5/qnetworkreply.html} {QNetworkReply} object created
    in \l {TmcClient::} {authenticate()} emits the signal
    \l {http://doc.qt.io/qt-5/qnetworkreply.html#finished}
    {QNetworkReply::finished}. If an error occurred during authentication,
    the slot emits the signal \l TmcClient::TMCError.
 */
void TmcClient::authenticationFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit TMCError(QString("Login failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        QSettings settings("TestMyQt", "TMC");
        settings.setValue("username", "");
        settings.setValue("password", "");
        settings.deleteLater();
        reply->deleteLater();
        return;
    }
    qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
    qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    qDebug() << json.toJson();
    auto name = json.object();
    accessToken = name["access_token"].toString();
    qDebug() << accessToken;

    emit loginFinished();
    reply->deleteLater();
}

/*!
    By the time the slot is called the \l {http://doc.qt.io/qt-5/qnetworkreply.html}
    {QNetworkReply} object pointed to by \a reply should contain a
    particular TMC course's exercise list as a JSON document. Assuming there has been
    no errors during the download phase, the JSON document is processed and each
    of the exercises are added to the \l Course object pointed to by \a course with
    a call to \l Course::addExercise().
*/
void TmcClient::exerciseListReplyFinished(QNetworkReply *reply, Course *course)
{
    if (reply->error()) {
        qDebug() << "Error at Exercise list reply finished";
        emit TMCError(QString("Failed to download exercise list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
        return;
    }

    qDebug() << "Exercise List:";
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    qDebug() << json.toJson();

    QJsonObject jsonObj = json.object();
    QJsonObject jsonCourse = jsonObj["course"].toObject();
    QJsonArray exercises = jsonCourse["exercises"].toArray();
    for (int i = 0; exercises.size() > i; i++) {
        QJsonObject exercise = exercises[i].toObject();
        // qDebug() << exercise["name"].toString();

        Exercise ex(exercise["id"].toInt(), exercise["name"].toString());
        ex.setChecksum(exercise["checksum"].toString());
        course->addExercise(ex);
        qDebug() << ex.getId() << ex.getName();
        qDebug() << course->getExercise(ex.getId()).getName();
        qDebug() << exercise["checksum"].toString();

    }
    emit exerciseListReady(course);

    reply->deleteLater();
}

/*!
    The primary purpose of the function is to extract a successfully downloaded
    zip archive to an appropriate target directory. \l
    {http://doc.qt.io/qt-5/qnetworkreply.html} {QNetworkReply} \a reply contains the zip
    archive data and \a ex identifies the relevant \l Exercise object.
 */
void TmcClient::exerciseZipReplyFinished(QNetworkReply *reply, Exercise *ex)
{
    if (reply->error()) {
        // One of the downloads was cancelled by the user
        if( reply->error() == QNetworkReply::OperationCanceledError ) {
            qDebug() << "Cancelled download:" << reply->url();
        } else {
            qDebug() << "Error at exerciseListReplyFinished";
            emit TMCError(QString("Zip download error %1: %2")
                          .arg(reply->errorString(), reply->error()));
        }
        reply->close();
        reply->deleteLater();
        return;
    }

    QBuffer storageBuff;
    storageBuff.setData(reply->readAll());
    QuaZip zip(&storageBuff);
    if (!zip.open(QuaZip::mdUnzip))
        emit TMCError("Error opening exercise zip file!");

    QStringList extracted = JlCompress::extractDir(&storageBuff, ex->getLocation());
    if (extracted.isEmpty()) {
        emit TMCError("Error unzipping exercise files!");
    } else {
        emit exerciseZipReady(ex);
    }

    reply->close();
    reply->deleteLater();
    return;
}

/*!
    \fn void TmcClient::TMCError(QString errorString)

    The signal is the error reporting mechanism of class \l TmcClient.
    The \a errorString contains a human-readable error message.
 */

/*!
    \fn void TmcClient::exerciseListReady(Course *course)

    Emitted by \l {TmcClient::} {exerciseListReplyFinished()} after the exercise
    list returned by the TMC server has been successfully processed. The exercise
    list can be obtained from the \l Course object parameter \a course using
    \l {Course::} {getExercises()}.
 */

/*!
    \fn void TmcClient::exerciseZipReady(Exercise *ex)

    Emitted by \l {TmcClient::} {exerciseZipReplyFinished()} after successfully
    extracting the downloaded exercise zip archive to the appropriate directory.
    Parameter \a ex identifies the \l Exercise object the signal was emitted for.
 */
