#include "testnetworkaccessmanager.h"
#include "testnetworkreply.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

TestNetworkAccessManager::TestNetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{

}

void TestNetworkAccessManager::setReply(TestNetworkReply *reply)
{
    m_reply = reply;
}

QNetworkReply *TestNetworkAccessManager::createRequest( Operation op,
                                                             const QNetworkRequest &req,
                                                             QIODevice *outgoingData )
{
    Q_UNUSED(op)
    Q_UNUSED(req)
    Q_UNUSED(outgoingData)
    return m_reply;
}
