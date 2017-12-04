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

#include "course.h"
#include "organization.h"

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
    QNetworkReply* getExerciseZip(Exercise *ex);
    void getCourseList(Organization org);
    void getOrganizationList();

    bool isAuthorized();
    bool isAuthenticated();

signals:
    void TMCError(QString errorString);
    void authorizationFinished(QString clientId, QString clientSecret);
    void authenticationFinished(QString accessToken);
    void exerciseListReady();
    void exerciseZipReady(Exercise *ex);
    void organizationListReady(QList<Organization> organizations);
    void courseListReady(Organization organization);
    void accessTokenNotValid();
    void closeDownloadWindow();

private slots:
    void authorizationReplyFinished (QNetworkReply *reply);
    void authenticationReplyFinished (QNetworkReply *reply);
    void organizationListReplyFinished(QNetworkReply *reply);
    void courseListReplyFinished(QNetworkReply *reply, Organization org);
    void exerciseListReplyFinished (QNetworkReply *reply, Course *course);
    void exerciseZipReplyFinished (QNetworkReply *reply, Exercise *ex);

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
