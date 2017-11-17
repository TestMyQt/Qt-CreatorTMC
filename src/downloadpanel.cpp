/*!
    \class DownloadPanel
    \inmodule src
    \brief The \c DownloadPanel class is a top-level \l
    {http://doc.qt.io/qt-5/qwidget.html} {QWidget} (a window) that is
    used to display the download progress of TMC exercises.

    \image ss_DownloadPanel.png "Screenshot: DownloadPanel window"

    Each of the TMC exercises is packed and transferred as a separate zip
    archive. It's therefore typical to have multiple concurrent downloads.
    The \c DownloadPanel window displays a separate \l
    {http://doc.qt.io/qt-5/qprogressbar.html} {QProgressBar} for each
    of them.

    Class \c DownloadPanel doesn't have any data transfer functionality of its own.
    It just gives a visual indication of the downloads initiated by
    \c TmcClient::getExerciseZip().

    \note A slight problem with the progress bars is their inaccuracy. This is
    due to the signal \c QNetworkReply::downloadProgress(qint64 bytesReceived,
    qint64 bytesTotal). Each time it is emitted for an incomplete download
    the value of \a bytesTotal is -1 rather than the true download size. Whenever
    \a bytesTotal is -1 a progress bar has to resort to using the crude
    approximation \c AVERAGE_DOWNLOAD_SIZE.
*/

#include <QDebug>
#include <QTimer>

#include "downloadpanel.h"

#define TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS 3000
// Perhaps with a different font the height of the widgets could be reduced
// to less than 15.
#define FIXED_WIDGET_HEIGHT 15
// The server doesn't seem to reveal the sizes of the files being downloaded,
// so the following crude estimate is needed.
static const int AVERAGE_DOWNLOAD_SIZE = 1500000;

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

/*!
    Adds an info or status label to the bottom of the \c DownloadPanel window.
    The info label is used for displaying short messages to the user. The
    function should only be called once after adding the rest of the widgets.
 */
void DownloadPanel::addInfoLabel()
{
    if( doneAddingWidgets ) {
        qDebug() << "DownloadPanel::addInfoLabel() was called even though "
            "doneAddingWidgets was false";
        return;
    }

    // Just to create a bit of empty space between the info label and the
    // last progress bar
    /*
    QLabel *blankLabel = new QLabel;
    blankLabel->setFixedHeight( FIXED_WIDGET_HEIGHT );
    layout->addWidget( blankLabel, 2 * numberOfProgressBars, 0 );
    */

    infoLabel = new QLabel( "Downloading files..." );
    infoLabel->setAlignment( Qt::AlignCenter );
    // infoLabel->setStyleSheet( "background: white" );
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
    QLabel *label = new QLabel( downloadName );
    label->setFixedHeight( FIXED_WIDGET_HEIGHT );
    layout->addWidget( label, 2 * numberOfProgressBars, 0 );
    progressBarLabels.append( label );

    // Add the progress bar
    QProgressBar *bar = new QProgressBar;
    bar->setMinimum( 0 );
    bar->setMaximum( AVERAGE_DOWNLOAD_SIZE );
    bar->setValue( 0 );
    bar->setFixedHeight( FIXED_WIDGET_HEIGHT );
    layout->addWidget( bar, 2 * numberOfProgressBars + 1, 0 );
    progressBars.append( bar );

    // Add the button
    QPushButton *button = new QPushButton( "❎" );
    button->setFixedHeight( FIXED_WIDGET_HEIGHT );
    button->setFixedWidth( FIXED_WIDGET_HEIGHT );
    layout->addWidget( button, 2 * numberOfProgressBars + 1, 1 );
    connect( button, SIGNAL( clicked() ), this, SLOT( cancelDownload() ) );
    downloadCancelButtons.append( button );

    ++numberOfProgressBars;
}

/*!
    Adds \a reply to the \c replies list of type \c {QList<QNetworkReply *>}.
    There's also the list \c progressBars which is of the same size as
    \c replies. Each element in \c replies corresponds to the element at the
    same index in \c progressBars.
 */
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
