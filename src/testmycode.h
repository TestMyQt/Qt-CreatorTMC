#pragma once
#include <ui_downloadscreen.h>
#include "testmycode_global.h"
#include "tmcclient.h"
#include "downloadpanel.h"
#include "settingswidget.h"

#include <extensionsystem/iplugin.h>

#include <QObject>
#include <QString>

namespace TestMyCodePlugin {
namespace Internal {

class TestMyCode : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TestMyCode.json")

public:
    TestMyCode();
    ~TestMyCode();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    // Downloadform
    void onDownloadOkClicked();
    void refreshDownloadList();

private:

    SettingsWidget *settingsWidget;
    void showSettingsWidget();

    // Downloadform
    Ui::downloadform *downloadform;
    QWidget *downloadWidget;
    void showDownloadWidget();

    // tmcClient
    TmcClient tmcClient;
    void runTMC();
    void setDefaultCourse();
    void openProject(Exercise *ex);
    void displayTMCError(QString errorText);
    void clearCredentials();
    void doAuth(QString username, QString password, bool savePassword);
    void handleLoginResponse(QString accessToken);
    void handleAuthResponse(QString clientId, QString clientSecret);

    // DownloadPanel
    DownloadPanel *downloadPanel;
};

} // namespace Internal
} // namespace TestMyCodePlugin
