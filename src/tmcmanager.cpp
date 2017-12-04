#include "tmcmanager.h"
#include "testmycodeconstants.h"

#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QDebug>
#include <QAction>
#include <QFuture>
#include <QPushButton>
#include <QMap>

using Core::ProgressManager;
using Core::FutureProgress;

TmcManager::TmcManager(QObject *parent) : QObject(parent)
{
    m_updateTimer.setSingleShot(false);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() { updateExercises(); });

    // Initialize download window
    downloadWidget = new QWidget;
    downloadform = new Ui::downloadform;
    downloadform->setupUi(downloadWidget);

    // Signal-Slot for download window
    connect(downloadform->okbutton, &QPushButton::clicked, this, &TmcManager::onDownloadOkClicked);
    connect(downloadform->cancelbutton, &QPushButton::clicked, this, [=](){
        downloadWidget->close();
    });
}

TmcManager::~TmcManager()
{
    if (m_updateProgress.isRunning())
        m_updateProgress.reportFinished();

    if (m_downloadProgress.isRunning())
        m_downloadProgress.reportFinished();
}

void TmcManager::setTmcClient(TmcClient *client)
{
    m_client = client;
    connect(m_client, &TmcClient::exerciseListReady, this, &TmcManager::handleUpdates);
    connect(m_client, &TmcClient::exerciseZipReady, this, &TmcManager::handleZip);
    connect(m_client, &TmcClient::exerciseUpdateReady, this, &TmcManager::handleUpdates);
}

void TmcManager::setSettings(SettingsWidget *settings)
{
    m_settings = settings;
}

void TmcManager::handleUpdates(Course *course)
{
    QMap<int, Exercise> updates = course->getExercises();
    downloadform->exerciselist->clear();

    foreach (Exercise ex, updates.values()) {
        if (ex.isDownloaded()) {
            continue;
        }
        QListWidgetItem* item = new QListWidgetItem(ex.getName(), downloadform->exerciselist);
        item->setData(Qt::UserRole, QVariant::fromValue<Exercise*>(&ex));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked);
    }

    m_updateProgress.reportFinished();
    downloadWidget->show();
}

void TmcManager::handleZip(Exercise *ex)
{

}

void TmcManager::updateExercises()
{
    Course* activeCourse = m_settings->getActiveCourse();
    if (!activeCourse) {
        qDebug() << "No active course!";
        return;
    }

    m_updateProgress.setProgressRange(0, activeCourse->getExercises().size());
    FutureProgress *progress =
            ProgressManager::addTask(m_updateProgress.future(),
                                     tr("Updating TestMyCode exercises"),
                                     TestMyCodePlugin::Constants::TASK_INDEX);
    m_updateProgress.reportStarted();
    m_client->getExerciseList(activeCourse);
}

bool TmcManager::lastUpdateSuccessful()
{
    return m_updateSuccessful;
}

int TmcManager::updateInterval()
{
    return m_updateInterval;
}

void TmcManager::setUpdateInterval(int updateInterval)
{
    m_updateInterval = updateInterval;
    m_updateTimer.setInterval(updateInterval * 60000);
    if (m_updateTimer.interval() > 0)
        m_updateTimer.start();

}

void TmcManager::showDownloadWidget()
{
    if (!m_client->isAuthenticated()) {
        m_settings->showLoginWidget();
        return;
    }
    updateExercises();
    downloadWidget->show();
}

void TmcManager::onDownloadOkClicked()
{
    auto exerciseList = downloadform->exerciselist;

    Course *activeCourse = m_settings->getActiveCourse();
    if (m_settings->getWorkingDirectory() == "" || activeCourse->getName() == "")
        return;

    QString saveDirectory = m_settings->getWorkingDirectory() + "/" + activeCourse->getName();

    downloadPanel = new DownloadPanel();

    for (int idx = 0; idx < exerciseList->count(); idx++) {
        auto item = exerciseList->item(idx);
        if (item->checkState() == Qt::Checked) {
            Exercise ex = item->data(Qt::UserRole).value<Exercise>();
            activeCourse->getExercise(ex);
            ex->setLocation(saveDirectory);
            downloadPanel->addWidgetsToDownloadPanel(ex->getName());
            QNetworkReply* reply = m_client->getExerciseZip(ex);

            connect(reply, &QNetworkReply::downloadProgress,
                downloadPanel, &DownloadPanel::networkReplyProgress);
            connect(reply, &QNetworkReply::finished,
                downloadPanel, &DownloadPanel::httpFinished);

            downloadPanel->addReplyToList(reply);
        }
    }

    downloadPanel->addInfoLabel();
    downloadPanel->sanityCheck(); // Should be removed at some point
    downloadPanel->show();
}
