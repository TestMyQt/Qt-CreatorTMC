#ifndef TMCMANAGER_H
#define TMCMANAGER_H

#include "tmcclient.h"
#include "settingswidget.h"
#include "submitwidget.h"
#include "downloadpanel.h"
#include "tmcresultreader.h"

#include <autotest/testresult.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>

#include <ui_downloadscreen.h>

#include <QFutureWatcher>
#include <QObject>
#include <QTimer>

using namespace TestMyCode;
using namespace Autotest::Internal;
using namespace ProjectExplorer;

class TmcManager : public QObject
{
    Q_OBJECT
public:
    explicit TmcManager(TmcClient *client, QObject *parent = nullptr);
    ~TmcManager();

    void testActiveProject();
    void submitActiveExercise();
    void openActiveCoursePage();

    int updateInterval();
    bool lastUpdateSuccessful();

    void setSettings(SettingsWidget *settings);
    void loadSettings();

signals:

public slots:
    void displayTMCError(QString errorText);

    void onStartupProjectChanged(Project *project);
    Exercise getProjectExercise(Project *project);

    void setUpdateInterval(int interval);
    void updateExercises();
    void askSubmit(const Project *project);

    void handleUpdates(Course *updatedCourse, QList<Exercise> newExercises);
    void appendToDownloadWindow(QList<Exercise> exercises);

private slots:
    void onDownloadOkClicked();
    void handleZip(QByteArray zipData, Exercise ex);

private:

    TmcClient *m_client;
    SettingsWidget *m_settings;

    Project *m_activeProject = nullptr;

    void openExercise(Exercise ex);
    void showSubmitWidget(const Project *project);

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
