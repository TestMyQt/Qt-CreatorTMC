#ifndef TMCCLIENT_H
#define TMCCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QUrlQuery>

#include "course.h"
#include "downloadpanel.h"

class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = 0);
    void authenticate(QString username, QString password, bool savePassword);
    void getUserInfo();
    void getExerciseList(Course* course);
    void getExerciseZip(Exercise *ex, DownloadPanel *downloadPanel);
    Course * getCourse();
    void loadAccessToken();

signals:
    void loginFinished();
    void exerciseListReady();
    void exerciseZipReady(Exercise *ex);
    void accessTokenNotValid();
    void closeDownloadWindow();

public slots:
    void authenticationFinished (QNetworkReply *reply);
    void exerciseListReplyFinished (QNetworkReply *reply);
    void exerciseZipReplyFinished (QNetworkReply *reply, Exercise *ex);

private:
    QNetworkAccessManager manager;
    QString accessToken;
    Course *m_course;
    bool checkRequestStatus(QNetworkReply *reply);

};

#endif // TMCCLIENT_H
