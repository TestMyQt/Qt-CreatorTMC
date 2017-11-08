#include <QDebug>
#include <QTimer>

#include "downloadpanel.h"

#define TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS 7500
static const int AVERAGE_DOWNLOAD_SIZE = 1400000;

DownloadPanel::DownloadPanel( QWidget *parent ) : QWidget( parent )
{
    layout = new QGridLayout;
    numberOfProgressBars = 0;
    doneAddingWidgets = false;

    setLayout( layout );
    setWindowTitle( "Download Panel" );
}

DownloadPanel::~DownloadPanel()
{

}

// Should only be called once after adding the rest of the widgets
void DownloadPanel::addInfoLabel()
{
    if( doneAddingWidgets ) {
        qDebug() << "DownloadPanel::addInfoLabel() was called even though "
            "doneAddingWidgets was false";
        return;
    }

    // Just to create a bit of empty space between the info label and the
    // last progress bar
    QLabel *blankLabel = new QLabel;
    blankLabel->setFixedHeight( 15 );
    layout->addWidget( blankLabel, 2 * numberOfProgressBars, 0 );

    infoLabel = new QLabel( "Downloading files..." );
    infoLabel->setAlignment( Qt::AlignCenter );
    infoLabel->setStyleSheet( "background: yellow" );
    infoLabel->setFixedHeight( 50 );
    layout->addWidget( infoLabel, 2 * numberOfProgressBars + 1, 0 );

    doneAddingWidgets = true;
}

void DownloadPanel::addWidgetsToDownloadPanel( QString downloadName )
{
    if( doneAddingWidgets ) {
        qDebug() << "DownloadPanel::addWidgetsToDownloadPanel() was called "
            "even though doneAddingWidgets was false";
        return;
    }

    // Add the label
    QLabel *label = new QLabel( downloadName );
    layout->addWidget( label, 2 * numberOfProgressBars, 0 );
    progressBarLabels.append( label );

    // Add the progress bar
    QProgressBar *bar = new QProgressBar;
    bar->setMinimum( 0 );
    bar->setMaximum( AVERAGE_DOWNLOAD_SIZE );
    bar->setValue( 0 );
    layout->addWidget( bar, 2 * numberOfProgressBars + 1, 0 );
    progressBars.append( bar );

    // Add the button
    QPushButton *button = new QPushButton( "❎" );
    button->setMaximumHeight( 15 );
    button->setMaximumWidth( 15 );
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

    infoLabel->setText(
        "Done!\n(window should close in a moment)" );
    QTimer::singleShot( TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS,
        this, SLOT( close() ) );
}

void DownloadPanel::cancelDownload()
{
    for( int i = 0; i < downloadCancelButtons.size(); i++ ) {
        if( downloadCancelButtons[ i ] == QObject::sender() ) {
            downloadCancelButtons[ i ]->setEnabled( false );
            replies[ i ]->abort();
            progressBars[ i ]->setValue( 0 );
            progressBars[ i ]->setTextVisible( false );
            infoLabel->setText( "Cancelled download of\n" +
                progressBarLabels[ i ]->text() );
            progressBarLabels[ i ]->setText( "(cancelled)" );
            // TODO: Output the values of the min and max
        }
    }
}
