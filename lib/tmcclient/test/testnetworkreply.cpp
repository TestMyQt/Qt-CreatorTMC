#include "testnetworkreply.h"
#include <QNetworkReply>
#include <QTimer>

struct TestNetworkReplyPrivate
{
    QByteArray content;
    qint64 offset;
};

TestNetworkReply::TestNetworkReply( QObject *parent )
    : QNetworkReply(parent)
{
    d = new TestNetworkReplyPrivate;
}

TestNetworkReply::~TestNetworkReply()
{
    delete d;
}

void TestNetworkReply::setStatusOK()
{
    setHttpStatusCode(200, "OK");
}

void TestNetworkReply::setStatusUnauthorized()
{
    setHttpStatusCode(403, "Forbidden");
    setError(NetworkError::AuthenticationRequiredError, QString("Forbidden"));
}

void TestNetworkReply::setHttpStatusCode( int code, const QByteArray &statusText )
{
    setAttribute( QNetworkRequest::HttpStatusCodeAttribute, code );
    if ( statusText.isNull() )
        return;

    setAttribute( QNetworkRequest::HttpReasonPhraseAttribute, statusText );
}

void TestNetworkReply::setHeader( QNetworkRequest::KnownHeaders header, const QVariant &value )
{
    QNetworkReply::setHeader( header, value );
}

void TestNetworkReply::setContentType( const QByteArray &contentType )
{
    setHeader(QNetworkRequest::ContentTypeHeader, contentType);
}

void TestNetworkReply::setContent( const QString &content )
{
    setContent(content.toUtf8());
}

void TestNetworkReply::setContent( const QByteArray &content )
{
    d->content = content;
    d->offset = 0;

    open(ReadOnly | Unbuffered);
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(content.size()));

    QMetaObject::invokeMethod(this, "readyRead", Qt::DirectConnection);
    QMetaObject::invokeMethod(this, "finished", Qt::DirectConnection);
}

void TestNetworkReply::abort()
{
    // NOP
}

qint64 TestNetworkReply::bytesAvailable() const
{
    return d->content.size() - d->offset + QIODevice::bytesAvailable();
}

bool TestNetworkReply::isSequential() const
{
    return true;
}

qint64 TestNetworkReply::readData(char *data, qint64 maxSize)
{
    if (d->offset >= d->content.size()) {
        return -1;
    }

    qint64 number = qMin(maxSize, d->content.size() - d->offset);
    memcpy(data, d->content.constData() + d->offset, number);
    d->offset += number;

    return number;
}
