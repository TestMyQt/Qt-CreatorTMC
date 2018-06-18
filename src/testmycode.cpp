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
    TmcOutputPane::destroy();
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

    auto downloadUpdateAction = new QAction(tr("Download/Update"), this);
    Command *downloadUpdateCmd = ActionManager::registerAction(downloadUpdateAction,
                                                               Constants::DOWNLOAD_UPLOAD_ACTION_ID,
                                                               Context(Core::Constants::C_GLOBAL));
    auto courseAction = new QAction(this);
    courseAction->setEnabled(false);
    courseAction->setVisible(false);
    Command *courseCmd = ActionManager::registerAction(courseAction,
                                                       Constants::COURSE_ID,
                                                       Context(Core::Constants::C_GLOBAL));

    auto submitAction = new QAction(tr("Submit"), this);
    Command *submitCmd = ActionManager::registerAction(submitAction,
                                                       Constants::SUBMIT_ACTION_ID,
                                                       Context(Core::Constants::C_GLOBAL));

    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));
    settingsCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+S")));
    downloadUpdateCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+D")));
    submitCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+B")));

    // Create context menu with actions
    ActionContainer *menu = ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("TestMyCode"));
    ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(loginCmd);
    menu->addAction(settingsCmd);
    menu->addAction(downloadUpdateCmd);
    menu->addAction(submitCmd);
    menu->addAction(courseCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = ActionManager::actionContainer(Core::Constants::M_WINDOW);
    ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    // Initialize settings window
    settingsWidget = new SettingsWidget(&tmcClient);

    connect(loginAction, &QAction::triggered, settingsWidget, &SettingsWidget::showLoginWidget);
    connect(settingsAction, &QAction::triggered, settingsWidget, &SettingsWidget::display);
    connect(settingsWidget, &SettingsWidget::enableDownloadSubmit, this, [=](bool enable){
       downloadUpdateAction->setEnabled(enable);
       submitAction->setEnabled(enable);
    });

    connect(settingsWidget, &SettingsWidget::activeCourseChanged, this, [=](Course *course){
        if (!course || course->getId() == -1) {
            courseAction->setEnabled(false);
            courseAction->setVisible(false);
            return;
        }

        courseCmd->action()->setText("Active project: " + course->getTitle());
        courseAction->setEnabled(true);
        courseAction->setVisible(true);
    });

    // Initialize output pane
    TmcOutputPane::instance();

    // TmcManager
    m_tmcManager = new TmcManager(&tmcClient);
    m_tmcManager->setSettings(settingsWidget);

    connect(downloadUpdateAction, &QAction::triggered, m_tmcManager, &TmcManager::updateExercises);
    // Run tests
    connect(tmcAction, &QAction::triggered, m_tmcManager, &TmcManager::testActiveProject);
    // Submit active project
    connect(submitAction, &QAction::triggered, m_tmcManager, &TmcManager::submitActiveExercise);
    // Open browser for course page
    connect(courseAction, &QAction::triggered, m_tmcManager, &TmcManager::openActiveCoursePage);

    QNetworkAccessManager *m = new QNetworkAccessManager;
    tmcClient.setNetworkManager(m);

    settingsWidget->loadSettings();

    // Disable/Enable Download/Update and Submit buttons
    if (!tmcClient.isAuthenticated()) {
        downloadUpdateAction->setDisabled(true);
        submitAction->setDisabled(true);
    }

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

} // namespace Internal
} // namespace TestMyCodePlugin
