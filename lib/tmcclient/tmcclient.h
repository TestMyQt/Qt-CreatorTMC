#ifndef TMCCLIENT_H
#define TMCCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QUrlQuery>

#include "course.h"

class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = 0);

    void setNetworkManager(QNetworkAccessManager *m);
    void setAccessToken(QString token);
    void setClientId(QString id);
    void setClientSecret(QString secret);

    void authorize();
    void authenticate(QString username, QString password);
    void getUserInfo();
    void getExerciseList(Course *course);
    QNetworkReply* getExerciseZip(Exercise *ex);

    bool isAuthorized();
    bool isAuthenticated();

signals:
    void TMCError(QString errorString);
    void authorizationFinished(QString clientId, QString clientSecret);
    void authenticationFinished(QString accessToken);
    void exerciseListReady(Course *course);
    void exerciseZipReady(Exercise *ex);
    void accessTokenNotValid();
    void closeDownloadWindow();

private slots:
    void authorizationReplyFinished (QNetworkReply *reply);
    void authenticationReplyFinished (QNetworkReply *reply);
    void exerciseListReplyFinished (QNetworkReply *reply, Course *course);
    void exerciseZipReplyFinished (QNetworkReply *reply, Exercise *ex);

private:
    QNetworkRequest buildRequest(QUrl url);
    QNetworkReply* doGet(QUrl url);
    QNetworkAccessManager *manager;
    QString accessToken;
    QString clientId;
    QString clientSecret;
    bool checkRequestStatus(QNetworkReply *reply);
};

#endif // TMCCLIENT_H
