#pragma once
#include <ui_loginscreen.h>
#include <ui_downloadscreen.h>
#include "testmycode_global.h"
#include "tmcclient.h"
#include <extensionsystem/iplugin.h>
#include <QWidget>

#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>

#include <extensionsystem/iplugin.h>
#include <utils/hostosinfo.h>
#include <utils/fileutils.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QFileInfo>

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
    void on_login_cancelbutton_clicked();
    void on_login_loginbutton_clicked();
    // Downloadform
    void on_download_cancelbutton_clicked();
    void on_download_okbutton_clicked();

private:
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
    void runTMC();
};

} // namespace Internal
} // namespace TestMyCodePlugin
