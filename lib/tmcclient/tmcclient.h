#ifndef TMCCLIENT_H
#define TMCCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QUrlQuery>
#include <QBuffer>

#include "exercise.h"
#include "course.h"
#include "organization.h"
#include "submission.h"

class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = 0);

    void setNetworkManager(QNetworkAccessManager *m);
    void setAccessToken(QString token);
    void setClientId(QString id);
    void setClientSecret(QString secret);
    void setServerAddress(QString address);

    void authorize();
    void authenticate(QString username, QString password);
    void getUserInfo();
    void getExerciseList(Course *course);
    QNetworkReply* getExerciseZip(Exercise ex);
    void postExerciseZip(Exercise ex, QByteArray zipData);
    void getSubmissionStatus(int submissionId);
    void getCourseList(Organization org);
    void getOrganizationList();

    bool isAuthorized();
    bool isAuthenticated();

signals:
    void TMCError(QString errorString);
    void authorizationFinished(QString clientId, QString clientSecret);
    void authenticationFinished(QString accessToken);
    void exerciseListReady(Course *course, QList<Exercise> courseList);
    void exerciseZipReady(QByteArray zipData, Exercise ex);
    void exerciseSubmitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal);
    void exerciseSubmitReady(Exercise ex, QString submissionUrl);
    void organizationListReady(QList<Organization> organizations);
    void courseListReady(Organization organization);
    void submissionStatusReady(Submission submission);
    void accessTokenNotValid();

private slots:
    void authorizationReplyFinished (QNetworkReply *reply);
    void authenticationReplyFinished (QNetworkReply *reply);
    void organizationListReplyFinished(QNetworkReply *reply);
    void courseListReplyFinished(QNetworkReply *reply, Organization org);
    void exerciseListReplyFinished (QNetworkReply *reply, Course *course);
    void exerciseZipReplyFinished (QNetworkReply *reply, Exercise ex);
    void zipSubmitReplyFinished(QNetworkReply *reply, Exercise ex);
    void submissionStatusReplyFinished(QNetworkReply *reply, int submissionId);


private:
    QNetworkRequest buildRequest(QUrl url);
    QNetworkReply* doGet(QUrl url);
    QNetworkAccessManager *manager;
    QString accessToken;
    QString clientId;
    QString clientSecret;
    QString serverAddress;
    bool checkRequestStatus(QNetworkReply *reply);
};

#endif // TMCCLIENT_H
