/*!
    \class TmcClient
    \inmodule lib/tmcclient
    \inheaderfile tmcclient.h
    \brief Class \l TmcClient is the primary means of communication with
        the TMC server.

    \l TmcClient uses \l {https://oauth.net/2/} {OAuth2} for authentication by
    sending a POST request to the TMC server with the parameters \c client_id,
    \c client_secret, \c username, \c password and \c grant_type. Here's a
    description of the parameters:
    \list
    \li \c client_id and \c client_secret are used to authorize the client application.
    They are fetched from the TMC server at first startup and are cached in
    application \l {http://doc.qt.io/qt-5/qsettings.html} {QSettings}.
    \li \c username and \c password are the TMC credentials for user authentication.
    \li \c grant_type with the string value \c "password" indicates we are
    using the \l {https://oauth.net/2/} {OAuth2} password authentication flow.
    \endlist

    If the login information is correct, the TMC server responds with a JSON
    document that contains an access token. \l TmcClient uses the access token
    for further communication with the TMC server.
*/

/*!
    \fn void TmcClient::TMCError(QString errorString)

    Signal \c TMCError is the primary error reporting mechanism of \l TmcClient.
    The parameter \a errorString should contain a concise error message which
    should be displayed to the user by the slot(s) connected to the signal.
*/

/*!
    \fn void TmcClient::authenticationFinished(QString accessToken)

    Emitted after an authentication attempt. If the attempt failed, parameter
    \a accessToken will have an empty string as its value. In case of a successful
    authentication attempt \a accessToken will contain the access token received
    from the TMC server.

    The signal is connected to a private slot in \l {TestMyCodePlugin::Internal::} {TestMyCode}
    which stores the access token for later use.
*/

/*!
    \fn void TmcClient::authorizationFinished(QString clientId, QString clientSecret)

    Emitted when the job begun by \l {TmcClient::} {authorize()} is finished.
    The slot connected to the signal should store \a clientId and \a clientSecret
    for future use.
*/

/*!
    \fn void TmcClient::exerciseListReady(Course *course)

    Emitted when the job begun by \l {TmcClient} {getExerciseList()} is finished.
    The \a course parameter contains the list of exercises as a collection of type
    \c QList<Exercise> which can be retrieved with \l {Course::} {getExercises()}.
*/

/*!
    \fn void TmcClient::exerciseZipReady(Exercise *ex)

    The signal is emitted once the compressed exercise file requested from the
    TMC server by \l {TmcClient::} {getExerciseZip()} has completed successfully
    and the contents have been extracted. Parameter \a ex is the \l Exercise
    object that corresponds to the downloaded exercise.

    The signal is connected to a private function in \l {TestMyCodePlugin::Internal::}
    {TestMyCode} that opens the exercise \a ex as a project in Qt Creator.
*/

#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QBuffer>

#include <quazip/JlCompress.h>

#include "exercise.h"
#include "course.h"

TmcClient::TmcClient(QObject *parent) : QObject(parent)
{
}

/*!
    The QtCreatorTMC plugin uses a single \l
    {http://doc.qt.io/qt-5/qnetworkaccessmanager.html} {QNetworkAccessManager}
    object for managing network communications. The object is initialized in
    \l TestMyCodePlugin::Internal::TestMyCode::initialize(). \c setNetworkManager()
    is called from \l {TestMyCodePlugin::Internal::TestMyCode::} {initialize()}
    with the address of the \l {http://doc.qt.io/qt-5/qnetworkaccessmanager.html}
    {QNetworkAccessManager} object as the value for parameter \a m.
*/
void TmcClient::setNetworkManager(QNetworkAccessManager *m)
{
    manager = m;
}

/*!
    Sets the value of the private \c accessToken member of \l TmcClient
    to \a token.
*/
void TmcClient::setAccessToken(QString token)
{
    accessToken = token;
}

/*!
    Sets the value of the private \c clientId member of \l TmcClient
    to \a id.
*/
void TmcClient::setClientId(QString id)
{
    clientId = id;
}

/*!
    Sets the value of the private \c clientSecret member of \l TmcClient
    to \a secret.
*/
void TmcClient::setClientSecret(QString secret)
{
    clientSecret = secret;
}

/*!
    An authorized \l TmcClient object has to have a \l {https://oauth.net/2/}
    {OAuth 2.0} client ID and client secret.

    Returns \c true if authorization has occured and \c false otherwise.
*/
bool TmcClient::isAuthorized()
{
    return !(clientId.isEmpty() || clientSecret.isEmpty());
}

/*!
    Tests whether successful authentication has occured. The sign of
    successful authention is that the client has an access token.

    Returns \c true if authentication has occured and \c false otherwise.
 */
bool TmcClient::isAuthenticated()
{
    return !accessToken.isEmpty();
}

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
    Gets a client ID and client secret from the TMC server. For this to work
    the \l TmcClient object needs to have an access token which is received
    from the TMC server after a successful login attempt.
*/
void TmcClient::authorize()
{
    QUrl url("https://tmc.mooc.fi/api/v8/application/qtcreator_plugin/credentials.json");
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        authorizationReplyFinished(reply);
    });
}

/*!
    Gets called when the user clicks the \tt {Log in} button in the
    \tt {TMC Login} dialog. The values for the parameters \a username
    and \a password are those entered by the user in the dialog.
 */
void TmcClient::authenticate(QString username, QString password)
{

    if (!isAuthorized()) {
        emit TMCError(QString("Login failed: "
                              "no client id/secret available"));
        emit authenticationFinished("");
        return;
    }

    QUrl url("https://tmc.mooc.fi/oauth/token");

    QString grantType = "password";
    QUrlQuery params;
    params.addQueryItem("client_id", clientId);
    params.addQueryItem("client_secret", clientSecret);
    params.addQueryItem("username", username);
    params.addQueryItem("password", password);
    params.addQueryItem("grant_type", grantType);

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        authenticationReplyFinished(reply);
    });
}

/*!
    Downloads the TMC exercise specified by the \l Exercise parameter \a ex.
    The exercise is delivered from the TMC server as a single compressed file.
    After the download completes the contents of the compressed file are
    automatically extracted to the appropriate directory (determining what
    this directory is involves \l {Exercise::} {getLocation()}).
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
    Downloads the exercise list associated with the \l Course pointer \a course
    from the TMC server. The items in the list will be stored in \a course as
    \l Exercise objects.
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

void TmcClient::authorizationReplyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit TMCError(QString("Client authorization failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
    setClientId(json["application_id"].toString());
    setClientSecret(json["secret"].toString());

    emit authorizationFinished(clientId, clientSecret);
}

void TmcClient::authenticationReplyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit TMCError(QString("Login failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        emit authenticationFinished("");
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

    emit authenticationFinished(accessToken);
    reply->deleteLater();
}

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
