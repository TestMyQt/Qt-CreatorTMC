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

void TmcClient::setNetworkManager(QNetworkAccessManager *m)
{
    manager = m;
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


void TmcClient::loadAccessToken()
{
    QSettings settings("TestMyQt", "TMC");
    accessToken = settings.value("accessToken", "").toString();
    settings.deleteLater();
}

bool TmcClient::checkRequestStatus(QNetworkReply *reply)
{
    if (reply->error()) {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 403) {
            emit accessTokenNotValid();
            return "false";
        }
        return "false";
    }
    return "true";
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

void TmcClient::authenticationFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        emit TMCError(QString("Login failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        accessToken = "";
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
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("accessToken", accessToken);
    settings.deleteLater();

    emit loginFinished();
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
