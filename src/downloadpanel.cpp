#include <QDebug>
#include <QTimer>

#include "downloadpanel.h"

DownloadPanel::DownloadPanel( QWidget *parent ) : QWidget( parent )
{
    layout = new QGridLayout;
    nextGridLayoutRow = 0;

    setLayout( layout );
    setWindowTitle( "Download Panel" );
}

DownloadPanel::~DownloadPanel()
{

}

void DownloadPanel::addLabelToDownloadPanel( QString labelText )
{
    layout->addWidget( new QLabel( labelText ), nextGridLayoutRow, 0 );
    ++nextGridLayoutRow;
}

void DownloadPanel::addProgressBarToDownloadPanel()
{
    QProgressBar *bar = new QProgressBar;
    progressBars << bar;
    bar->setMinimum( 0 );
    layout->addWidget( bar, nextGridLayoutRow, 0 );
    ++nextGridLayoutRow;
}

void DownloadPanel::addReplyToList( QNetworkReply *reply )
{
    replies << reply;
}

/*

DownloadPanel::DownloadPanel(
    QList<QUrl *> downloadURLs, QDir downloadDir, QWidget *parent )
    :
    QWidget( parent ), downloadURLs( downloadURLs ),
    downloadDir( downloadDir )
{
    removeUselessURLs();

    layout = new QGridLayout;

    for( int i = 0; i < this->downloadURLs.size(); i++ ) {
        progressBarLabels << new QLabel( this->downloadURLs[ i ]->toString() );
        layout->addWidget( progressBarLabels[ i ], 2 * i, 0 );

        progressBars << new QProgressBar;
        progressBars[ i ]->setMinimum( 0 );
        layout->addWidget( progressBars[ i ], 2 * i + 1, 0 );

        downloadCancelButtons << new QPushButton( "❎" );
        downloadCancelButtons[ i ]->setMaximumHeight( 25 );
        downloadCancelButtons[ i ]->setMaximumWidth( 25 );
        layout->addWidget( downloadCancelButtons[ i ], 2 * i + 1, 1 );

        connect( downloadCancelButtons[ i ], SIGNAL( clicked() ),
            this, SLOT( cancelDownload() ) );
    }

    if( !this->downloadURLs.size() ) {
        qDebug() << "No files to download; closing Download Panel window";
        QTimer::singleShot( 10, this, SLOT( close() ) );
    }

    this->setLayout( layout );
    this->setWindowTitle( "Download Panel" );

    run();
}

void DownloadPanel::run()
{
    createDownloadDirAsNeeded();
    prepareFiles();
    startRequests();
}

void DownloadPanel::createDownloadDirAsNeeded()
{
    if( !downloadDir.exists() ) {
        qDebug() << "INFO: Download directory" << downloadDir.absolutePath() <<
            "does not exist. Creating the directory.";

        if( !downloadDir.mkpath( downloadDir.absolutePath() ) ) {
            qDebug() << "ABORTING: Attempt to create the directory" <<
                downloadDir.absolutePath() << "failed";
            abort();
        }
    }
}

void DownloadPanel::prepareFiles()
{
    for( int i = 0; i < downloadURLs.size(); i++ ) {
        QString fileName = downloadURLs[ i ]->fileName();

        // It has already been established that the file name is not
        // an empty string
        fileName.prepend( downloadDir.path() + '/' );

        if( QFile::exists( fileName ) )
            QFile::remove( fileName );

        downloadFiles << openFileForWriting( fileName );
        if( !downloadFiles[ i ] ) {
            qDebug() << "ABORTING: Could not open file"
                << downloadFiles[ i ]->fileName() << "for writing";
            abort();
        }
    }
}

QFile *DownloadPanel::openFileForWriting( const QString &fileName )
{
    QScopedPointer<QFile> filePtr( new QFile( fileName ) );
    if( !filePtr->open( QIODevice::WriteOnly ) ) {
        return Q_NULLPTR;
    }
    return filePtr.take();
}

void DownloadPanel::startRequests()
{
    for( int i = 0; i < downloadURLs.size(); i++ ) {
        QUrl url = *downloadURLs[ i ];
        QNetworkReply *reply = manager.get( QNetworkRequest( url ) );
        replies << reply;
        connect( replies[ i ], &QNetworkReply::finished,
            this, &DownloadPanel::httpFinished );
        connect( replies[ i ], &QIODevice::readyRead,
            this, &DownloadPanel::httpReadyRead );
        connect( replies[ i ], &QNetworkReply::downloadProgress,
            this, &DownloadPanel::networkReplyProgress );
    }
}

#define FIND_SENDER_INDEX \
int senderIndex = -1; \
for( int i = 0; i < replies.size(); i++ ) \
    if( replies[ i ] == QObject::sender() ) { \
        senderIndex = i; \
        break; \
    } \
if( senderIndex == -1 ) { \
    qDebug() << "ABORTING: A problem occurred in finding the sender index"; \
    abort(); }

void DownloadPanel::httpFinished()
{
    FIND_SENDER_INDEX

    QFile *file = downloadFiles[ senderIndex ];
    // qDebug() << "Completed download of file" << file->fileName();

    file->close();
    delete file;
    downloadFiles[ senderIndex ] = Q_NULLPTR;

    replies[ senderIndex ]->deleteLater();
    replies[ senderIndex ] = Q_NULLPTR;

    // Grey out the cancel button of the download
    downloadCancelButtons[ senderIndex ]->setEnabled( false );

    closeWindowIfAllDownloadsComplete();
}

void DownloadPanel::httpReadyRead()
{
    FIND_SENDER_INDEX

    if( downloadFiles[ senderIndex ] ) {
        downloadFiles[ senderIndex ]->write( replies[ senderIndex ]->readAll() );
    }
    else {
        qDebug() << "ABORTING: No file available for downloaded data";
        abort();
    }
}

void DownloadPanel::networkReplyProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    FIND_SENDER_INDEX

    progressBars[ senderIndex ]->setMaximum( bytesTotal );
    progressBars[ senderIndex ]->setValue( bytesReceived );
}

void DownloadPanel::removeUselessURLs()
{
    bool anElementWasRemoved;
    for( int i = 0; i < downloadURLs.size(); i += ( anElementWasRemoved ? 0 : 1 ) ) {
        anElementWasRemoved = false;

        if( downloadURLs[ i ]->fileName().isEmpty() ) {
            qDebug() << "WARNING: Removing URL that lacks the file name part:" <<
                downloadURLs[ i ]->toString();
            downloadURLs.removeAt( i );
            anElementWasRemoved = true;
        }
    }
}

void DownloadPanel::cancelDownload()
{
    for( int i = 0; i < downloadCancelButtons.size(); i++ ) {
        if( downloadCancelButtons[ i ] == QObject::sender() ) {
            qDebug() << "Clicked on button with index" << i;
        }
    }
}

void DownloadPanel::closeWindowIfAllDownloadsComplete()
{
    for( int i = 0; i < downloadFiles.size(); i++ ) {
        if( downloadFiles[ i ] ) {
            return;
        }
    }

    QTimer::singleShot( 1000, this, SLOT( close() ) );
}

*/
