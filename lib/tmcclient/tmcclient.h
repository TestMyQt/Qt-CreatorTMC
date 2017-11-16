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
    void authenticate(QString username, QString password, bool savePassword);
    void getUserInfo();
    void loadAccessToken();
    void getExerciseList(Course *course);
    QNetworkReply* getExerciseZip(Exercise *ex);

signals:
    void TMCError(QString errorString);
    void loginFinished();
    void exerciseListReady(Course *course);
    void exerciseZipReady(Exercise *ex);
    void accessTokenNotValid();
    void closeDownloadWindow();

public slots:
    void authenticationFinished (QNetworkReply *reply);
    void exerciseListReplyFinished (QNetworkReply *reply, Course *course);
    void exerciseZipReplyFinished (QNetworkReply *reply, Exercise *ex);

private:
    QNetworkRequest buildRequest(QUrl url);
    QNetworkReply* doGet(QUrl url);
    QNetworkAccessManager *manager;
    QString accessToken;
    bool checkRequestStatus(QNetworkReply *reply);
};

#endif // TMCCLIENT_H
