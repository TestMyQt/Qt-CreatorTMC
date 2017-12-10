/*!
    \namespace TestMyCodePlugin
    \inmodule src
*/

/*!
    \namespace TestMyCodePlugin::Internal
    \inmodule src
*/

/*!
    \class TestMyCodePlugin::Internal::TestMyCode
    \inmodule src
    \inheaderfile testmycode.h
    \brief \l TestMyCode is the foundation of the QtCreatorTMC project
        in that it is the class for the Qt Creator plugin itself.

    Like all Qt Creator plugins, \l TestMyCode inherits \l
    {https://doc-snapshots.qt.io/qtcreator-extending/extensionsystem-iplugin.html}
    {IPlugin}.
*/

#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"
#include "tmcoutputpane.h"
#include "tmcrunner.h"
#include "course.h"

#include <QDebug>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QAction>

#include <QList>

#include <QtPlugin>
#include <extensionsystem/pluginmanager.h>

using namespace TestMyCodePlugin::Internal;

namespace TestMyCodePlugin {
namespace Internal {

TestMyCode::TestMyCode()
{
    // Create your members
}

TestMyCode::~TestMyCode()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool TestMyCode::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    auto tmcAction = new QAction(tr("Test project"), this);
    Core::Command *tmcCmd = Core::ActionManager::registerAction(tmcAction, Constants::TMC_ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));

    auto loginAction = new QAction(tr("Login"), this);
    Core::Command *loginCmd = Core::ActionManager::registerAction(loginAction, Constants::LOGIN_ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));

    auto settingsAction = new QAction(tr("Settings"), this);
    Core::Command *settingsCmd = Core::ActionManager::registerAction(settingsAction, Constants::SETTINGS_ACTION_ID,
                                                                     Core::Context(Core::Constants::C_GLOBAL));

    auto downloadAction = new QAction(tr("Download"), this);
    Core::Command *downloadCmd = Core::ActionManager::registerAction(downloadAction, Constants::DOWNLOAD_ACTION_ID,
                                                                     Core::Context(Core::Constants::C_GLOBAL));
    auto updateAction = new QAction(tr("Update"), this);
    Core::Command *updateCmd = Core::ActionManager::registerAction(updateAction, Constants::UPDATE_ACTION_ID,
                                                                   Core::Context(Core::Constants::C_GLOBAL));

    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));
    settingsCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+S")));
    downloadCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+D")));
    updateCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+U")));

    // Connect trigger to a function
    connect(tmcAction, &QAction::triggered, this, &TestMyCode::runTMC);
    connect(settingsAction, &QAction::triggered, this, &TestMyCode::showSettingsWidget);
    connect(loginAction, &QAction::triggered, this,  [=](){
        settingsWidget->showLoginWidget();
    });
    connect(downloadAction, &QAction::triggered, this, [=](){
        m_tmcManager->showDownloadWidget();
    });
    connect(updateAction, &QAction::triggered, this, [=](){
        m_tmcManager->updateExercises();
    });

    // Create context menu with actions
    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("TestMyCode"));
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(settingsCmd);
    menu->addAction(loginCmd);
    menu->addAction(downloadCmd);
    menu->addAction(updateCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = Core::ActionManager::actionContainer(Core::Constants::M_WINDOW);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    addAutoReleasedObject(TmcOutputPane::instance());

    // Initialize settings window
    settingsWidget = new SettingsWidget;
    settingsWidget->setTmcClient(&tmcClient);

    connect(loginAction, &QAction::triggered, this, [=](){
        settingsWidget->showLoginWidget();
    });
    connect(settingsWidget, &SettingsWidget::tmcCliLocationChanged, this, [=](const QString &location){
        tmcCliLocation = location;
    });

    // TmcManager
    m_tmcManager = new TmcManager;
    m_tmcManager->setTmcClient(&tmcClient);
    m_tmcManager->setSettings(settingsWidget);

    // TmcClient
    connect(&tmcClient, &TmcClient::TMCError, this, &TestMyCode::displayTMCError);

    QNetworkAccessManager *m = new QNetworkAccessManager;
    tmcClient.setNetworkManager(m);

    return true;
}

void TestMyCode::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag TestMyCode::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void TestMyCode::showSettingsWidget()
{
    if (!tmcClient.isAuthenticated()) {
        settingsWidget->showLoginWidget();
        return;
    }
    settingsWidget->display();
}

void TestMyCode::runTMC()
{
    TMCRunner *runner = TMCRunner::instance();
    runner->setTmcCliLocation(tmcCliLocation);
    runner->runOnActiveProject();
}

void TestMyCode::openProject(Exercise *ex)
{
    Q_UNUSED(ex)
}

void TestMyCode::displayTMCError(QString errorText)
{
    QMessageBox::critical(nullptr, "TMC", errorText, QMessageBox::Ok);
}

} // namespace Internal
} // namespace TestMyCodePlugin
