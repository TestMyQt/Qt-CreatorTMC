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

void TmcClient::setNetworkManager(QNetworkAccessManager *m)
{
    manager = m;
}

void TmcClient::setAccessToken(QString token)
{
    accessToken = token;
}

void TmcClient::setClientId(QString id)
{
    clientId = id;
}

void TmcClient::setClientSecret(QString secret)
{
    clientSecret = secret;
}

void TmcClient::setServerAddress(QString address)
{
    if (address.endsWith("/"))
        address.remove(address.length()-1, 1);
    serverAddress = address;
}

bool TmcClient::isAuthorized()
{
    return !(clientId.isEmpty() || clientSecret.isEmpty());
}

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

bool TmcClient::checkRequestStatus(QNetworkReply *reply)
{
    if (reply->error()) {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 403) {
            emit accessTokenNotValid();
            return false;
        }
        return false;
    }
    return true;
}

void TmcClient::authorize()
{
    QUrl url(serverAddress + "/api/v8/application/qtcreator_plugin/credentials.json");
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        authorizationReplyFinished(reply);
    });
}

void TmcClient::authenticate(QString username, QString password)
{

    if (!isAuthorized()) {
        emit TMCError(QString("Login failed: "
                              "no client id/secret available"));
        emit authenticationFinished("");
        return;
    }

    QUrl url(serverAddress + "/oauth/token");

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

void TmcClient::getOrganizationList()
{
    QUrl url(QString(serverAddress + "/api/v8/org.json"));
    QNetworkReply *reply = doGet(url);
    connect(reply, &QNetworkReply::finished, this, [=](){
        organizationListReplyFinished(reply);
    });
}

void TmcClient::getCourseList(Organization org)
{
    QUrl url(QString(serverAddress + "/api/v8/core/org/%1/courses").arg(org.getSlug()));
    QNetworkReply *reply = doGet(url);
    connect(reply, &QNetworkReply::finished, this, [=](){
        courseListReplyFinished(reply, org);
    });
}

QNetworkReply* TmcClient::getExerciseZip(Exercise *ex)
{
    QUrl url(QString(serverAddress + "/api/v8/core/exercises/%1/download").arg(ex->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseZipReplyFinished(reply, ex);
    });

    return reply;
}


void TmcClient::getExerciseList(Course *course)
{
    QUrl url(serverAddress + "/api/v8/core/courses/" + QString::number(course->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseListReplyFinished(reply, course);
    });
}

void TmcClient::getUserInfo()
{
    QUrl url(serverAddress + "/api/v8/users/current");
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

void TmcClient::organizationListReplyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error at Organization list reply finished";
        emit TMCError(QString("Failed to download organization list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
    }
    QList<Organization> organizations;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QJsonArray orgJson = json.array();
    for (int i = 0; orgJson.size() > i; i++) {
        QJsonObject org = orgJson[i].toObject();
        organizations.append(Organization(org["name"].toString(), org["slug"].toString()));
    }
    emit organizationListReady(organizations);
    reply->deleteLater();
}

void TmcClient::courseListReplyFinished(QNetworkReply *reply, Organization org)
{
    if (reply->error()) {
        qDebug() << "Error at Course list reply finished";
        emit TMCError(QString("Failed to download course list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
    }
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QJsonArray coursesJson = json.array();
    foreach (QJsonValue courseJson, coursesJson) {
        org.addCourse(Course::fromJson(courseJson.toObject()));
    }
    emit courseListReady(org);
    reply->deleteLater();
}

void TmcClient::exerciseListReplyFinished(QNetworkReply *reply, Course *course)
{
    if (!checkRequestStatus(reply)) {
        qDebug() << "Error at Exercise list reply finished";
        emit TMCError(QString("Failed to download exercise list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
        emit closeDownloadWindow();
        return;
    }

    qDebug() << "Exercise List:";
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());

    QJsonObject jsonObj = json.object();
    QJsonObject jsonCourse = jsonObj["course"].toObject();
    QJsonArray exercises = jsonCourse["exercises"].toArray();
    for (int i = 0; exercises.size() > i; i++) {
        QJsonObject exercise = exercises[i].toObject();
        Exercise ex(exercise["id"].toInt(), exercise["name"].toString());
        ex.setChecksum(exercise["checksum"].toString());
        ex.setDlDate(exercise["deadline"].toString());
        // Workaround to not add same exercises over and over again
        // needs to be reworked to allow updating
        if (course->getExercise(exercise["id"].toInt()).getId() == -1) {
            ex.saveSettings(course->getName());
            course->addExercise(ex);
        }
        qDebug() << ex.getId() << ex.getName();
        qDebug() << course->getExercise(ex.getId()).getName();
        qDebug() << exercise["checksum"].toString();

    }
    emit exerciseListReady();

    reply->deleteLater();
}

void TmcClient::exerciseZipReplyFinished(QNetworkReply *reply, Exercise *ex)
{
    if (!checkRequestStatus(reply)) {
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
        emit closeDownloadWindow();
    }

    reply->close();
    reply->deleteLater();
    return;
}
