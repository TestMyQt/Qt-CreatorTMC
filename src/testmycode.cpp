#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"

#include <ui_loginscreen.h>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>

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

    auto action = new QAction(tr("Login"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    // Shortcut
    // cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+K")));
    connect(action, &QAction::triggered, this, &TestMyCode::showLoginWidget);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);

    menu->menu()->setTitle(tr("TestMyCode"));
    menu->addAction(cmd);

    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    // Initialize login window
    loginWidget = new QWidget;
    login = new Ui::loginform;
    login->setupUi(loginWidget);
    // Signal-Slot for login window
    QObject::connect(login->cancelbutton, SIGNAL(clicked(bool)), this, SLOT(on_cancelbutton_clicked()));
    QObject::connect(login->loginbutton, SIGNAL(clicked(bool)), this, SLOT(on_loginbutton_clicked()));

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

void TestMyCode::showLoginWidget()
{
    loginWidget->show();
}

} // namespace Internal
} // namespace TestMyCodePlugin

void TestMyCodePlugin::Internal::TestMyCode::on_cancelbutton_clicked()
{
    loginWidget->close();
}

void TestMyCodePlugin::Internal::TestMyCode::on_loginbutton_clicked()
{
    // TODO: Authentication
    QString username = login->usernameinput->text();
    QString password = login->passwordinput->text();
    tmcClient.authenticate(username, password);
}
