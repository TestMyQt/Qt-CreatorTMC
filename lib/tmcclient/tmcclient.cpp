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
    QUrl url("https://tmc.mooc.fi/api/v8/application/qtcreator_plugin/credentials.json");
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

void TmcClient::getCourseList(QString address)
{
    QString org = address.section("/", -1);
    QUrl url(QString("https://tmc.mooc.fi/api/v8/core/org/%1/courses").arg(org));
    QNetworkReply *reply = doGet(url);
    connect(reply, &QNetworkReply::finished, this, [=](){
        courseListReplyFinished(reply);
    });
}

QNetworkReply* TmcClient::getExerciseZip(Exercise *ex)
{
    QUrl url(QString("https://tmc.mooc.fi/api/v8/core/exercises/%1/download").arg(ex->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseZipReplyFinished(reply, ex);
    });

    return reply;
}


void TmcClient::getExerciseList(Course *course)
{
    QUrl url("https://tmc.mooc.fi/api/v8/core/courses/" + QString::number(course->getId()));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseListReplyFinished(reply, course);
    });
}

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

void TmcClient::courseListReplyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error at Course list reply finished";
        emit TMCError(QString("Failed to download course list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        reply->deleteLater();
    }
    QMap<QString, int> courses;
    QJsonDocument json = QJsonDocument::fromJson((reply->readAll()));
    QJsonArray coursesJson = json.array();
    for (int i = 0; coursesJson.size() > i; i++) {
        QJsonObject course = coursesJson[i].toObject();
        courses.insert(course["name"].toString(), course["id"].toInt());
    }
    emit courseListReady(courses);
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
