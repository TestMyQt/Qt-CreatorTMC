#ifndef TMCMANAGER_H
#define TMCMANAGER_H

#include "tmcclient.h"
#include "settingswidget.h"
#include "submitwidget.h"

#include "exercisewidget.h"
#include "exercisemodel.h"

#include "downloadpanel.h"
#include "tmcresultreader.h"

#include <autotest/testresult.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>

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

    void onExerciseOpen(const Exercise &ex);

    void handleUpdates(Course *updatedCourse, QList<Exercise> newExercises);

private slots:
    void handleZip(QByteArray zipData, Exercise ex);

private:

    TmcClient *m_client;
    SettingsWidget *m_settings;

    Project *m_activeProject = nullptr;

    ExerciseWidget *m_exerciseWidget = nullptr;
    ExerciseModel *m_exerciseModel = nullptr;

    void showSubmitWidget(const Project *project);
    void showExerciseWidget(QList<Exercise> exercises);

    QFutureInterface<void> m_updateProgress;
    QFutureInterface<void> m_downloadProgress;

    QTimer m_updateTimer;

};

#endif // TMCMANAGER_H
