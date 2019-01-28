#ifndef DOWNLOADPANEL_H
#define DOWNLOADPANEL_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QNetworkReply>
#include <QPushButton>

class DownloadPanel : public QWidget
{
    Q_OBJECT

public:
    DownloadPanel(QWidget *parent = nullptr);
    ~DownloadPanel();
    void addReplyToList(QNetworkReply *reply);
    QNetworkReply *getRepliesListItem(int index);
    void sanityCheck();
    void addWidgetsToDownloadPanel(const QString &downloadName);
    void addInfoLabel();

public Q_SLOTS:
    void networkReplyProgress(qint64 bytesReceived, qint64 bytesTotal);
    void httpFinished();
    void cancelDownload();

private:
    void closeWindowIfAllDownloadsComplete();
    QGridLayout *layout;
    QList<QProgressBar *> progressBars;
    QList<QNetworkReply *> replies;
    int numberOfProgressBars;
    QList<QPushButton *> downloadCancelButtons;
    bool doneAddingWidgets;
    QList<QLabel *> progressBarLabels;
    QLabel *infoLabel;
};

#endif // DOWNLOADPANEL_H
