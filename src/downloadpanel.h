#ifndef DOWNLOADPANEL_H
#define DOWNLOADPANEL_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QNetworkReply>

/*
#include <QUrl>
#include <QFile>
#include <QPushButton>
#include <QDir>
*/

class DownloadPanel : public QWidget
{
    Q_OBJECT

public:
    DownloadPanel( QWidget *parent = 0 );
    ~DownloadPanel();
    void addLabelToDownloadPanel( QString labelText );
    void addProgressBarToDownloadPanel();
    void addReplyToList( QNetworkReply *reply );

private:
    QGridLayout *layout;
    int nextGridLayoutRow;
    QList<QProgressBar *> progressBars;
    QList<QNetworkReply *> replies;

/*
private:
    void run();
    void createDownloadDirAsNeeded();
    void prepareFiles();
    QFile *openFileForWriting( const QString &fileName );
    void startRequests();
    void removeUselessURLs();
    void closeWindowIfAllDownloadsComplete();

    QList<QLabel *> progressBarLabels;
    QList<QUrl *> downloadURLs;
    QList<QFile *> downloadFiles;
    QList<QPushButton *> downloadCancelButtons;
    QNetworkAccessManager manager;
    QDir downloadDir;

private slots:
    void httpFinished();
    void httpReadyRead();
    void networkReplyProgress( qint64 bytesReceived, qint64 bytesTotal );
    void cancelDownload();
*/
};

#endif // DOWNLOADPANEL_H
