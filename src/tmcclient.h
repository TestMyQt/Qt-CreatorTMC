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


class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = 0);
    void authenticate(QString username, QString password, bool savePassword);
    void getUserInfo();
    void getExerciseList(Course* course);
    Course * getCourse();
signals:
    void loginFinished();
    void exerciseListReady();

public slots:
    void authenticationFinished (QNetworkReply *reply);
    void exerciseListReplyFinished (QNetworkReply *reply);

private:
    QNetworkAccessManager manager;
    QString accessToken;
    Course *m_course;

};

#endif // TMCCLIENT_H
