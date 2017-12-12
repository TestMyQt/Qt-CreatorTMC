#include "tmcclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHttpPart>

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
        }
        reply->close();
        reply->deleteLater();
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

QNetworkReply* TmcClient::getExerciseZip(Exercise ex)
{
    QUrl url(QString(serverAddress + "/api/v8/core/exercises/%1/download").arg(ex.getId()));
    QNetworkReply *reply = doGet(url);
    connect(reply, &QNetworkReply::finished, this, [=](){
        exerciseZipReplyFinished(reply, ex);
    });

    return reply;
}

void TmcClient::postExerciseZip(Exercise ex, QByteArray zipData)
{
    QUrl url(QString(serverAddress + "/api/v8/core/exercises/%1/submissions").arg(ex.getId()));
    QNetworkRequest request = buildRequest(url);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"submission[file]\"; "
                                "filename=\"submission.zip\""));

    filePart.setBody(zipData);
    multiPart->append(filePart);

    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply); // delete multipart with reply

    connect(reply, &QNetworkReply::uploadProgress, this, [=](qint64 bytesSent, qint64 bytesTotal){
        emit exerciseSubmitProgress(ex, bytesSent, bytesTotal);
    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        zipSubmitReplyFinished(reply, ex);
    });
}

void TmcClient::getSubmissionStatus(int submissionId)
{
    QUrl url(serverAddress + "/api/v8/core/submissions/" + QString::number(submissionId));
    QNetworkReply *reply = doGet(url);

    connect(reply, &QNetworkReply::finished, this, [=](){
        submissionStatusReplyFinished(reply, submissionId);
    });
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
    if (!checkRequestStatus(reply)) {
        emit TMCError(QString("Client authorization failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
    setClientId(json["application_id"].toString());
    setClientSecret(json["secret"].toString());

    emit authorizationFinished(clientId, clientSecret);
}

void TmcClient::authenticationReplyFinished(QNetworkReply *reply)
{
    if (!checkRequestStatus(reply)) {
        emit TMCError(QString("Login failed: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        emit authenticationFinished("");
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
    if (!checkRequestStatus(reply)) {
        qDebug() << "Error at Organization list reply finished";
        emit TMCError(QString("Failed to download organization list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        return;
    }
    QList<Organization> organizations;
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QJsonArray orgJson = json.array();
    foreach (QJsonValue jsonVal, orgJson) {
        organizations.append(Organization::fromJson(jsonVal.toObject()));
    }
    emit organizationListReady(organizations);
    reply->deleteLater();
}

void TmcClient::courseListReplyFinished(QNetworkReply *reply, Organization org)
{
    if (!checkRequestStatus(reply)) {
        qDebug() << "Error at Course list reply finished";
        emit TMCError(QString("Failed to download course list: %1: %2")
                      .arg(reply->errorString(), reply->error()));
        return;
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
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());

    QJsonObject jsonObj = json.object();
    QJsonObject jsonCourse = jsonObj["course"].toObject();
    QJsonArray exercises = jsonCourse["exercises"].toArray();

    QList<Exercise> courseList;
    foreach (QJsonValue jsonVal, exercises) {
        courseList.append(Exercise::fromJson(jsonVal.toObject()));
    }

    emit exerciseListReady(course, courseList);

    reply->deleteLater();
}

void TmcClient::exerciseZipReplyFinished(QNetworkReply *reply, Exercise ex)
{
    if (!checkRequestStatus(reply)) {
        // One of the downloads was cancelled by the user
        if( reply->error() == QNetworkReply::OperationCanceledError ) {
            qDebug() << "Cancelled download:" << ex.getName();
        } else {
            qDebug() << "Error at exerciseListReplyFinished";
            emit TMCError(QString("Zip download error %1: %2")
                          .arg(reply->errorString(), reply->error()));
        }
        return;
    }

    emit exerciseZipReady(reply->readAll(), ex);
    reply->close();
    reply->deleteLater();
}

void TmcClient::zipSubmitReplyFinished(QNetworkReply *reply, Exercise ex)
{
    if (!checkRequestStatus(reply)) {
            emit TMCError(QString("Zip upload error %1: %2")
                          .arg(reply->errorString(), reply->error()));
            return;
    }
    QJsonObject submission = QJsonDocument::fromJson(reply->readAll()).object();
    QString submissionUrl = submission["submission_url"].toString();

    emit exerciseSubmitReady(ex, submissionUrl);
    reply->deleteLater();
}

void TmcClient::submissionStatusReplyFinished(QNetworkReply *reply, int submissionId)
{
    if (!checkRequestStatus(reply)) {
            emit TMCError(QString("Submission status update error %1: %2")
                          .arg(reply->errorString(), reply->error()));
            return;
    }

    QJsonObject jsonSubmission = QJsonDocument::fromJson(reply->readAll()).object();
    Submission submission = Submission::fromJson(submissionId, jsonSubmission);

    emit submissionStatusReady(submission);
    reply->deleteLater();
}
