#include <QDebug>
#include <QTimer>

#include "downloadpanel.h"

static const int TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS = 2000;
// Perhaps with a different font the height of the widgets could be reduced
// to less than 15.
static const int FIXED_WIDGET_HEIGHT = 15;
// The server doesn't seem to reveal the sizes of the files being downloaded,
// so the following crude estimate is needed.
static const int AVERAGE_DOWNLOAD_SIZE = 1500000;

DownloadPanel::DownloadPanel( QWidget *parent ) : QWidget( parent )
{
    layout = new QGridLayout( parent );
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

    infoLabel = new QLabel( "Downloading files...", this );
    infoLabel->setAlignment( Qt::AlignCenter );
    infoLabel->setFixedHeight( 35 );
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
    QLabel *label = new QLabel( downloadName, this );
    label->setFixedHeight( FIXED_WIDGET_HEIGHT );
    layout->addWidget( label, 2 * numberOfProgressBars, 0 );
    progressBarLabels.append( label );

    // Add the progress bar
    QProgressBar *bar = new QProgressBar( this );
    bar->setMinimum( 0 );
    bar->setMaximum( AVERAGE_DOWNLOAD_SIZE );
    bar->setValue( 0 );
    bar->setFixedHeight( FIXED_WIDGET_HEIGHT );
    layout->addWidget( bar, 2 * numberOfProgressBars + 1, 0 );
    progressBars.append( bar );

    // Add the button
    QPushButton *button = new QPushButton( "❎", this );
    button->setFixedHeight( FIXED_WIDGET_HEIGHT );
    button->setFixedWidth( FIXED_WIDGET_HEIGHT );
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

void DownloadPanel::networkReplyProgress(
    qint64 bytesReceived, qint64 bytesTotal )
{
    int senderIndex = replies.indexOf(reinterpret_cast<QNetworkReply *>
        (QObject::sender()));

    progressBars[ senderIndex ]->setMaximum(
        bytesTotal != -1 ? bytesTotal : AVERAGE_DOWNLOAD_SIZE );
    progressBars[ senderIndex ]->setValue( bytesReceived );
}

void DownloadPanel::httpFinished()
{
    int senderIndex = replies.indexOf(reinterpret_cast<QNetworkReply *>
        (QObject::sender()));

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

    infoLabel->setText( "Done!" );
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
            // Without the following statement the progress bar of a cancelled
            // download that has not yet started will look different from one
            // that has already started
            progressBars[ i ]->setMaximum( -1 );
            progressBars[ i ]->setTextVisible( false );
            infoLabel->setText( "Cancelled download of\n" +
                progressBarLabels[ i ]->text() );
            progressBarLabels[ i ]->setText( "(cancelled)" );
        }
    }

    closeWindowIfAllDownloadsComplete();
}
