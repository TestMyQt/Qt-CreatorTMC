#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TmcClient::TmcClient(QObject *parent ) : QObject(parent)
{
}

void TmcClient::authenticate(QString username, QString password)
{
    QString client_id = "8355b4a75a4191edfedeae7b074571278fd4987d4234c01569678b9ad11f526d";
    QString client_secret = "c2b1176a6189ceaa16cd51f805ef20ea6c993d36fdb76aa873ac35471d2df4f1";
    QString username_ = username;
    QString password_ = password;
    QString grant_type = "password";

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
        replyFinished(reply);
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

void TmcClient::replyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error at replyfinished";
        qDebug() << reply->errorString();
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
    }
    reply->deleteLater();
    getUserInfo();
}
