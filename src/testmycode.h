#pragma once
#include <ui_loginscreen.h>
#include <ui_downloadscreen.h>
#include <ui_settings.h>
#include "testmycode_global.h"
#include "tmcclient.h"
#include "downloadpanel.h"

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
    // Loginform
    void onLoginCancelClicked();
    void onLoginClicked();
    void onChangeServerClicked();
    // Downloadform
    void onDownloadCancelClicked();
    void onDownloadOkClicked();
    void refreshDownloadList();
    // Settings window
    void onSettingsOkClicked();
    void onSettingsBrowseClicked();

private:
    QString currentOrgName;
    QString currentOrgSlug;
    QString workingDirectory;

    // SettingsWindow
    Ui::settingsForm *settingsWindow;
    QWidget *settingsWidget;
    void showSettingsWidget();
    QString askSaveLocation();

    // Loginform
    Ui::loginform *login;
    QWidget *loginWidget;
    void showLoginWidget();

    // Downloadform
    Ui::downloadform *downloadform;
    QWidget *downloadWidget;
    void showDownloadWidget();

    // tmcClient
    TmcClient tmcClient;
    Course *m_activeCourse;
    void runTMC();
    void setDefaultCourse();
    void openProject(Exercise *ex);
    void displayTMCError(QString errorText);
    void clearCredentials();
    void doAuth(QString username, QString password, bool savePassword);
    void handleLoginResponse(QString accessToken);
    void handleAuthResponse(QString clientId, QString clientSecret);
    void handleOrganizationList(QMap<QString, QString> organizations);
    void handleCourseList(QMap<QString, int> courses);

    // ...
    DownloadPanel *downloadPanel;
};

} // namespace Internal
} // namespace TestMyCodePlugin
