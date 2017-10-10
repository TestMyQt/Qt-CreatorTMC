#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QUrlQuery>


class Authentication : public QObject
{
    Q_OBJECT
public:
    explicit Authentication(QObject *parent = 0);
    void doDownload();

signals:

public slots:
    void replyFinished (QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;

};

#endif // AUTHENTICATION_H
