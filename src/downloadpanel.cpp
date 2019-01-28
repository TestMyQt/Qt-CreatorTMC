/*!
    \class DownloadPanel
    \inmodule src
    \inheaderfile downloadpanel.h
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
    \c TestMyCode::on_download_okbutton_clicked().

    \note A slight problem with the progress bars is their inaccuracy. This is
    due to the signal \c QNetworkReply::downloadProgress(qint64 bytesReceived,
    qint64 bytesTotal). Each time it is emitted for an incomplete download
    the value of \a bytesTotal is -1 rather than the true download size. Whenever
    \a bytesTotal is -1 a progress bar has to resort to using the crude
    approximation \c AVERAGE_DOWNLOAD_SIZE.
*/

#include "downloadpanel.h"

#include <QDebug>
#include <QTimer>

static const int TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS = 2000;
// Perhaps with a different font the height of the widgets could be reduced
// to less than 15.
static const int FIXED_WIDGET_HEIGHT = 15;
// The server doesn't seem to reveal the sizes of the files being downloaded,
// so the following crude estimate is needed.
static const int AVERAGE_DOWNLOAD_SIZE = 1500000;

DownloadPanel::DownloadPanel(QWidget *parent)
    : QWidget(parent)
    , infoLabel(nullptr)
{
    layout = new QGridLayout(parent);
    numberOfProgressBars = 0;
    doneAddingWidgets = false;

    setLayout(layout);
    setWindowTitle("Download Panel");
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
    if (doneAddingWidgets) {
        qDebug() << "DownloadPanel::addInfoLabel() was called even though "
            "doneAddingWidgets was false";
        return;
    }

    infoLabel = new QLabel("Downloading files...", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setFixedHeight(35);
    layout->addWidget(infoLabel, 2 * numberOfProgressBars + 1, 0);

    doneAddingWidgets = true;
}

/*!
    Adds three widgets to the \c DownloadPanel window which have to do with
    a particular download. The widgets are a \c QLabel for naming the download,
    a \c QPushButton for cancelling the download and a \c QProgressBar for
    displaying the progress. The parameter \a downloadName is displayed in
    the \c QLabel.
 */
void DownloadPanel::addWidgetsToDownloadPanel(const QString &downloadName)
{
    if (doneAddingWidgets) {
        qDebug() << "DownloadPanel::addWidgetsToDownloadPanel() was called "
            "even though doneAddingWidgets was false";
        return;
    }

    // Add the label
    QLabel *label = new QLabel(downloadName, this);
    label->setFixedHeight(FIXED_WIDGET_HEIGHT);
    layout->addWidget(label, 2 * numberOfProgressBars, 0);
    progressBarLabels.append(label);

    // Add the progress bar
    QProgressBar *bar = new QProgressBar(this);
    bar->setMinimum(0);
    bar->setMaximum(AVERAGE_DOWNLOAD_SIZE);
    bar->setValue(0);
    bar->setFixedHeight(FIXED_WIDGET_HEIGHT);
    layout->addWidget(bar, 2 * numberOfProgressBars + 1, 0);
    progressBars.append(bar);

    // Add the button
    QPushButton *button = new QPushButton("❎", this);
    button->setFixedHeight(FIXED_WIDGET_HEIGHT);
    button->setFixedWidth(FIXED_WIDGET_HEIGHT);
    layout->addWidget(button, 2 * numberOfProgressBars + 1, 1);
    connect(button, &QPushButton::clicked, this, &DownloadPanel::cancelDownload);
    downloadCancelButtons.append(button);

    ++numberOfProgressBars;
}

/*!
    Appends \a reply to the end of list \c replies of type \c {QList<QNetworkReply *>}.
    There's also the \c QList instance variable \c progressBars which is of the same size as
    \c replies. Each element in \c replies corresponds to the element at the
    same index in \c progressBars.
 */
void DownloadPanel::addReplyToList(QNetworkReply *reply)
{
    replies.append(reply);
}

/*!
    Returns the \c QNetworkReply pointer specified by the \a index parameter. The
    pointer is an element in the \c QList instance variable \c replies.
 */
QNetworkReply *DownloadPanel::getRepliesListItem(int index)
{
    return replies.at(index);
}

/*!
    The signal \l {http://doc.qt.io/qt-5/qnetworkreply.html#downloadProgress}
    {QNetworkReply::downloadProgress} is connected to this
    slot in \c TestMyCode::on_download_okbutton_clicked(). \a bytesReceived
    indicates the number of bytes received thus far by the \c QNetworkReply
    object that emitted the signal. \a bytesTotal is the total size of
    the download. If the value of \a bytesTotal is -1, the total download size
    is unknown.
 */
void DownloadPanel::networkReplyProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int senderIndex = replies.indexOf(reinterpret_cast<QNetworkReply *>
        (QObject::sender()));

    progressBars[senderIndex]->setMaximum(
        bytesTotal != -1 ? bytesTotal : AVERAGE_DOWNLOAD_SIZE);
    progressBars[senderIndex]->setValue(bytesReceived);
}

/*!
    The signal \l {http://doc.qt.io/qt-5/qnetworkreply.html#finished}
    {QNetworkReply::finished} is connected to this slot in
    \c TestMyCode::on_download_okbutton_clicked().
 */
void DownloadPanel::httpFinished()
{
    int senderIndex = replies.indexOf(reinterpret_cast<QNetworkReply *>
        (QObject::sender()));

    // Grey out the cancel button of the download
    downloadCancelButtons[senderIndex]->setEnabled(false);

    replies[senderIndex]->deleteLater();
    replies[senderIndex] = nullptr;
    closeWindowIfAllDownloadsComplete();
}

/*!
    There's a one-to-one correspondence between the items of the \c QLists
    \c progressBars and \c replies. It follows that the sizes of the lists
    are the same. If this is not the case, the function issues a
    \l {http://doc.qt.io/qt-5/qtglobal.html#qDebug} {qDebug} warning.
 */
void DownloadPanel::sanityCheck()
{
    if( progressBars.size() != replies.size() ) {
        qDebug() << "WARNING: Problem in DownloadPanel object: "
            "progressBars.size() != replies.size()";
    }
}

/*!
    A function for automatically closing the \c DownloadPanel window.
 */
void DownloadPanel::closeWindowIfAllDownloadsComplete()
{
    for (int i = 0; i < replies.size(); i++) {
        if (replies[ i ]) { // Not a null pointer
            return;
        }
    }

    infoLabel->setText("Done!");
    QTimer::singleShot(TIME_BEFORE_WINDOW_CLOSES_AFTER_DOWNLOADS,
        this, &DownloadPanel::close);
}

/*!
    Each of the download cancel buttons in the \c DownloadPanel window are
    connected to this slot.
 */
void DownloadPanel::cancelDownload()
{
    for (int i = 0; i < downloadCancelButtons.size(); i++) {
        if (downloadCancelButtons[i] == QObject::sender()) {
            downloadCancelButtons[i]->setEnabled(false);
            replies[i]->abort();
            progressBars[i]->setValue(0);
            // Without the following statement the progress bar of a cancelled
            // download that has not yet started will look different from one
            // that has already started
            progressBars[i]->setMaximum(-1);
            progressBars[i]->setTextVisible(false);
            infoLabel->setText("Cancelled download of\n" +
                progressBarLabels[ i ]->text());
            progressBarLabels[i]->setText("(cancelled)");
        }
    }

    closeWindowIfAllDownloadsComplete();
}
