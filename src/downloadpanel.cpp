#include <QDebug>
#include <QTimer>

#include "downloadpanel.h"

static const int AVERAGE_DOWNLOAD_SIZE = 1400000;

DownloadPanel::DownloadPanel( QWidget *parent ) : QWidget( parent )
{
    layout = new QGridLayout;
    numberOfProgressBars = 0;

    setLayout( layout );
    setWindowTitle( "Download Panel" );
}

DownloadPanel::~DownloadPanel()
{

}

void DownloadPanel::addWidgetsToDownloadPanel( QString downloadName )
{
    // Add the label
    layout->addWidget( new QLabel( downloadName ),
        2 * numberOfProgressBars, 0 );

    // Add the progress bar
    QProgressBar *bar = new QProgressBar;
    bar->setMinimum( 0 );
    layout->addWidget( bar, 2 * numberOfProgressBars + 1, 0 );
    progressBars.append( bar );

    // Add the button
    QPushButton *button = new QPushButton( "❎" );
    button->setMaximumHeight( 20 );
    button->setMaximumWidth( 20 );
    layout->addWidget( button, 2 * numberOfProgressBars + 1, 1 );
    connect( button, SIGNAL( clicked() ), this, SLOT( cancelDownload() ) );
    downloadCancelButtons.append( button );

    ++numberOfProgressBars;
}

void DownloadPanel::addReplyToList( QNetworkReply *reply )
{
    replies.append( reply );
}

QNetworkReply *DownloadPanel::getRepliesListItem( int index )
{
    return replies.at( index );
}

#define FIND_SENDER_INDEX \
int senderIndex = -1; \
for( int i = 0; i < replies.size(); i++ ) \
    if( replies[ i ] == QObject::sender() ) { \
        senderIndex = i; \
        break; }

void DownloadPanel::networkReplyProgress(
    qint64 bytesReceived, qint64 bytesTotal )
{
    FIND_SENDER_INDEX

    progressBars[ senderIndex ]->setMaximum(
        bytesTotal != -1 ? bytesTotal : AVERAGE_DOWNLOAD_SIZE );
    progressBars[ senderIndex ]->setValue( bytesReceived );
}

void DownloadPanel::httpFinished()
{
    FIND_SENDER_INDEX

    // Grey out the cancel button of the download
    downloadCancelButtons[ senderIndex ]->setEnabled( false );

    replies[ senderIndex ] = Q_NULLPTR;
    closeWindowIfAllDownloadsComplete();
}

void DownloadPanel::sanityCheck()
{
    if( progressBars.size() != replies.size() ) {
        qDebug() << "WARNING: Problem in DownloadPanel object: "
            "progressBars.size() != replies.size()";
    }
}

void DownloadPanel::closeWindowIfAllDownloadsComplete()
{
    for( int i = 0; i < replies.size(); i++ ) {
        if( replies[ i ] ) { // Not a null pointer
            return;
        }
    }

    QTimer::singleShot( 1000, this, SLOT( close() ) );
}

void DownloadPanel::cancelDownload()
{
    for( int i = 0; i < downloadCancelButtons.size(); i++ ) {
        if( downloadCancelButtons[ i ] == QObject::sender() ) {
            qDebug() << "Clicked on button with index" << i;
        }
    }
}
