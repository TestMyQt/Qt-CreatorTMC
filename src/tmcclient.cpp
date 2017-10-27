#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>
#include <QSettings>

TmcClient::TmcClient(QObject *parent ) : QObject(parent)
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

    QNetworkReply *reply = manager.post(request, params.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        authenticationFinished(reply);
    });
}

void TmcClient::getExerciseList(QString courseId)
{
    QUrl url("https://tmc.mooc.fi/api/v8/courses/" + courseId + "/exercises");
    QNetworkRequest request(url);
    QString a = "Bearer ";
    request.setRawHeader(QByteArray("Authorization") , QByteArray(a.append(accessToken).toUtf8()));

    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseListReplyFinished(reply);
    });
}

void TmcClient::getUserInfo()
{
    QUrl url("https://tmc.mooc.fi/api/v8/users/current");
    QNetworkRequest request(url);
    QString a = "Bearer ";
    request.setRawHeader(QByteArray("Authorization") , QByteArray(a.append(accessToken).toUtf8()));

    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){
        qDebug() << reply->readAll();
        reply->deleteLater();
    });
}

void TmcClient::authenticationFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error at Authentication finished";
        QMessageBox::critical(NULL, "TMC", "Login failed", QMessageBox::Ok);
        QSettings settings("TestMyQt", "TMC");
        settings.setValue("username", "");
        settings.setValue("password", "");
        settings.deleteLater();
    } else {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        auto name = json.object();
        accessToken = name["access_token"].toString();
        qDebug() << accessToken;

        emit loginFinished();
    }
    reply->deleteLater();
    getUserInfo();
    getExerciseList("18");
}

void TmcClient::exerciseListReplyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error at Exercise list reply finished";
        QMessageBox::critical(NULL, "TMC", "Failed to Download exercise list", QMessageBox::Ok);
    } else {
        qDebug() << "Exercise List:";
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        QJsonArray exercises = json.array();
        qDebug() << exercises.size();
        for (int i = 0; exercises.size() > i; i++) {
            QJsonObject exercise = exercises[i].toObject();
            qDebug() << exercise["id"].toInt();
        }
    }
    reply->deleteLater();
}
