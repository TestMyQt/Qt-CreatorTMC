#ifndef TESTNETWORKREPLY_H
#define TESTNETWORKREPLY_H

#include <QNetworkReply>

class TestNetworkReply : public QNetworkReply
{
    Q_OBJECT

public:
    TestNetworkReply(QObject *parent = 0);
    ~TestNetworkReply();

    void setHttpStatusCode( int code, const QByteArray &statusText = QByteArray() );
    void setHeader( QNetworkRequest::KnownHeaders header, const QVariant &value );
    void setContentType( const QByteArray &contentType );

    void setContent( const QString &content );
    void setContent( const QByteArray &content );

    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

    void setStatusOK();
    void setStatusUnauthorized();
protected:
    qint64 readData(char *data, qint64 maxSize);

private:
    struct TestNetworkReplyPrivate *d;
};

#endif // TESTNETWORKREPLY_H
