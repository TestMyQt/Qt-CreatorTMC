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



class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = 0);
    void authenticate(QString username, QString password);
    void getUserInfo();

signals:
    void loginFinished();

public slots:
    void replyFinished (QNetworkReply *reply);

private:
    QNetworkAccessManager manager;
    QString accessToken;

};

#endif // TMCCLIENT_H
