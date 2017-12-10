#ifndef TMCMANAGER_H
#define TMCMANAGER_H

#include "tmcclient.h"
#include "settingswidget.h"
#include "downloadpanel.h"

#include <ui_downloadscreen.h>

#include <QFutureWatcher>
#include <QObject>
#include <QTimer>

class TmcManager : public QObject
{
    Q_OBJECT
public:
    explicit TmcManager(TmcClient *client, QObject *parent = nullptr);
    ~TmcManager();

    void setTmcClient(TmcClient *client);

    void testActiveProject();
    void submitActiveExercise();

    void setUpdateInterval(int interval);
    int updateInterval();
    bool lastUpdateSuccessful();

    void setSettings(SettingsWidget *settings);
    void loadSettings();

signals:
    void TMCError(QString errorString);

public slots:
    void showDownloadWidget();

    void updateExercises();
    void handleUpdates(Course *updatedCourse, QList<Exercise> newExercises);
    void appendToDownloadWindow(QList<Exercise> exercises);

private slots:
    void onDownloadOkClicked();
    void handleZip(QByteArray zipData, Exercise ex);


private:

    TmcClient *m_client;
    SettingsWidget *m_settings;

    // DownloadWidget
    Ui::downloadform *downloadform;
    QWidget *downloadWidget;

    // DownloadPanel
    DownloadPanel *downloadPanel;

    QFutureInterface<void> m_updateProgress;
    QFutureInterface<void> m_downloadProgress;

    QTimer m_updateTimer;
    bool m_updateSuccessful;

};

#endif // TMCMANAGER_H
