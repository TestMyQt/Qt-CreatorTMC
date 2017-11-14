#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>
#include <QSettings>
#include <QBuffer>

#include <quazip/JlCompress.h>

#include "exercise.h"
#include "course.h"

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
            return 0;
        }
        return 0;
    }
    return 1;
}

Course * TmcClient::getCourse()
{
    return m_course;
}

void TmcClient::getExerciseZip(Exercise *ex, DownloadPanel *downloadPanel)
{
    QUrl url(QString("https://tmc.mooc.fi/api/v8/core/exercises/%1/download").arg(ex->getId()));
    QNetworkRequest request(url);
    QString a = "Bearer ";
    request.setRawHeader(QByteArray("Authorization") , QByteArray(a.append(accessToken).toUtf8()));

    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseZipReplyFinished(reply, ex);
    });

    connect( reply, &QNetworkReply::downloadProgress,
        downloadPanel, &DownloadPanel::networkReplyProgress );
    connect( reply, &QNetworkReply::finished,
        downloadPanel, &DownloadPanel::httpFinished );

    downloadPanel->addReplyToList( reply );
}

void TmcClient::getExerciseList(Course *course)
{
    m_course = course;
    QUrl url("https://tmc.mooc.fi/api/v8/core/courses/" + QString::number(course->getId()));
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
        accessToken = ""; // for debugging purposes, mainly
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

        QSettings settings("TestMyQt", "TMC");
        settings.setValue("accessToken", accessToken);
        settings.deleteLater();

        emit loginFinished();
    }
    reply->deleteLater();
}

void TmcClient::exerciseListReplyFinished(QNetworkReply *reply)
{
    if (!checkRequestStatus(reply)) {
        qDebug() << "Error at Exercise list reply finished";
        emit closeDownloadWindow();
        QMessageBox::critical(NULL, "TMC", "Failed to Download exercise list", QMessageBox::Ok);
    } else {
        qDebug() << "Exercise List:";
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());

        QJsonObject jsonObj = json.object();
        QJsonObject jsonCourse = jsonObj["course"].toObject();
        m_course->setName(jsonCourse["name"].toString());
        QJsonArray exercises = jsonCourse["exercises"].toArray();
        m_course->saveSettings();
        for (int i = 0; exercises.size() > i; i++) {
            QJsonObject exercise = exercises[i].toObject();
            Exercise ex(exercise["id"].toInt(), exercise["name"].toString());
            ex.setChecksum(exercise["checksum"].toString());
            m_course->addExercise(ex);
            ex.saveSettings(m_course->getName());
            qDebug() << ex.getId() << ex.getName();
            qDebug() << m_course->getExercise(ex.getId()).getName();
            qDebug() << exercise["checksum"].toString();
        }
        emit exerciseListReady();
    }
    reply->deleteLater();
}

void TmcClient::exerciseZipReplyFinished(QNetworkReply *reply, Exercise *ex)
{
    if( checkRequestStatus(reply) ) { // No errors
        QBuffer storageBuff;
        storageBuff.setData(reply->readAll());
        QuaZip zip(&storageBuff);
        if (!zip.open(QuaZip::mdUnzip))
            qDebug() << "Error opening zip file!";
        QuaZipFile file(&zip);

        for (bool f = zip.goToFirstFile(); f; f = zip.goToNextFile()) {
            QuaZipFileInfo fileInfo;
            file.getFileInfo(&fileInfo);
            qDebug() << fileInfo.name;
        }

        // TODO:
        // Location of the extraction needs to be
        // WorkingDirectory/CourseName/ExerciseName/
        // This should be done when the working directory
        // is set on the Settings pane.
        JlCompress::extractDir(&storageBuff, ex->getLocation());
        emit exerciseZipReady(ex);
        emit closeDownloadWindow();
    }
    // One of the downloads was cancelled by the user
    else if( reply->error() == QNetworkReply::OperationCanceledError ) {
        qDebug() << "Cancelled download:" << reply->url();
    }
    // Some other error occured
    else {
        qDebug() << "Error at exerciseListReplyFinished";
        qDebug() << reply->error();
        QMessageBox::critical( NULL, "TMC",
            tr("Received %1").arg(reply->size()), QMessageBox::Ok);
    }

    reply->close();
    reply->deleteLater();
}
