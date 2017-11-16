#ifndef TESTNETWORKACCESSMANAGER_H
#define TESTNETWORKACCESSMANAGER_H

#include "testnetworkreply.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

class TestNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    TestNetworkAccessManager(QObject *parent = 0);
    void setReply(TestNetworkReply *reply);

protected:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData) Q_DECL_OVERRIDE;

private:
    TestNetworkReply *m_reply;
};

#endif // TESTNETWORKACCESSMANAGER_H
