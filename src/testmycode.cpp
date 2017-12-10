#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"
#include "tmcoutputpane.h"
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
using Core::ActionManager;
using Core::Command;
using Core::Context;
using Core::ActionContainer;

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
    Command *tmcCmd = ActionManager::registerAction(tmcAction, Constants::TMC_ACTION_ID,
                                                    Context(Core::Constants::C_GLOBAL));

    auto loginAction = new QAction(tr("Login"), this);
    Command *loginCmd = ActionManager::registerAction(loginAction, Constants::LOGIN_ACTION_ID,
                                                      Context(Core::Constants::C_GLOBAL));

    auto settingsAction = new QAction(tr("Settings"), this);
    Command *settingsCmd = ActionManager::registerAction(settingsAction, Constants::SETTINGS_ACTION_ID,
                                                         Context(Core::Constants::C_GLOBAL));

    auto downloadAction = new QAction(tr("Download"), this);
    Command *downloadCmd = ActionManager::registerAction(downloadAction, Constants::DOWNLOAD_ACTION_ID,
                                                         Context(Core::Constants::C_GLOBAL));
    auto updateAction = new QAction(tr("Update"), this);
    Command *updateCmd = ActionManager::registerAction(updateAction, Constants::UPDATE_ACTION_ID,
                                                       Context(Core::Constants::C_GLOBAL));
    auto submitAction = new QAction(tr("Submit"), this);
    Command *submitCmd = ActionManager::registerAction(submitAction, Constants::SUBMIT_ACTION_ID,
                                                       Context(Core::Constants::C_GLOBAL));

    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));
    settingsCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+S")));
    downloadCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+D")));
    updateCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+U")));
    submitCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+B")));

    // Create context menu with actions
    ActionContainer *menu = ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("TestMyCode"));
    ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(settingsCmd);
    menu->addAction(loginCmd);
    menu->addAction(downloadCmd);
    menu->addAction(updateCmd);
    menu->addAction(submitCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = ActionManager::actionContainer(Core::Constants::M_WINDOW);
    ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    addAutoReleasedObject(TmcOutputPane::instance());

    // Initialize settings window
    settingsWidget = new SettingsWidget(&tmcClient);

    connect(loginAction, &QAction::triggered, settingsWidget, &SettingsWidget::showLoginWidget);
    connect(settingsAction, &QAction::triggered, settingsWidget, &SettingsWidget::display);

    // TmcManager
    m_tmcManager = new TmcManager(&tmcClient);
    m_tmcManager->setSettings(settingsWidget);

    connect(downloadAction, &QAction::triggered, m_tmcManager, &TmcManager::showDownloadWidget);
    connect(updateAction, &QAction::triggered, m_tmcManager, &TmcManager::updateExercises);
    // Run tests
    connect(tmcAction, &QAction::triggered, m_tmcManager, &TmcManager::testActiveProject);
    // Submit active project
    connect(submitAction, &QAction::triggered, m_tmcManager, &TmcManager::submitActiveExercise);

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

void TestMyCode::displayTMCError(QString errorText)
{
    QMessageBox::critical(nullptr, "TMC", errorText, QMessageBox::Ok);
}

} // namespace Internal
} // namespace TestMyCodePlugin
