#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"
#include "tmcoutputpane.h"
#include "tmcrunner.h"

#include <ui_loginscreen.h>
#include <ui_downloadscreen.h>

#include <QApplication>
#include <QDebug>

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
#include <QObject>
#include <QString>
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>

#include <QSettings>

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

    auto downloadAction = new QAction(tr("Download"), this);
    Core::Command *downloadCmd = Core::ActionManager::registerAction(downloadAction, Constants::DOWNLOAD_ACTION_ID,
                                                                     Core::Context(Core::Constants::C_GLOBAL));
    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));
    downloadCmd->setDefaultKeySequence(QKeySequence(tr("Alt+D")));
    connect(loginAction, &QAction::triggered, this, &TestMyCode::showLoginWidget);
    connect(tmcAction, &QAction::triggered, this, &TestMyCode::runTMC);
    connect(downloadAction, &QAction::triggered, this, &TestMyCode::showDownloadWidget);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);

    menu->menu()->setTitle(tr("TestMyCode"));
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(loginCmd);
    menu->addAction(downloadCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = Core::ActionManager::actionContainer(Core::Constants::M_WINDOW);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    addAutoReleasedObject(TmcOutputPane::instance());

    // Initialize download window
    downloadWidget = new QWidget;
    downloadform = new Ui::downloadform;
    downloadform->setupUi(downloadWidget);

    // Initialize login window
    loginWidget = new QWidget;
    login = new Ui::loginform;
    login->setupUi(loginWidget);

    // Create settings
    QSettings settings("TestMyQt", "TMC");
    login->usernameinput->setText(settings.value("username", "").toString());
    login->passwordinput->setText(settings.value("password", "").toString());
    if (settings.value("savePasswordChecked").toString() == "true")
        login->savepasswordbox->setChecked("true");
    settings.deleteLater();

    // Signal-Slot for login window
    QObject::connect(login->cancelbutton, SIGNAL(clicked(bool)), this, SLOT(on_login_cancelbutton_clicked()));
    QObject::connect(login->loginbutton, SIGNAL(clicked(bool)), this, SLOT(on_login_loginbutton_clicked()));
    connect(&tmcClient, &TmcClient::loginFinished, this, &TestMyCode::on_login_cancelbutton_clicked);

    // Signal-Slot for download window
    QObject::connect(downloadform->cancelbutton, SIGNAL(clicked(bool)), this, SLOT(on_download_cancelbutton_clicked()));
    QObject::connect(downloadform->okbutton, SIGNAL(clicked(bool)), this, SLOT(on_download_okbutton_clicked()));

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

void TestMyCode::showDownloadWidget()
{
    downloadWidget->show();
}

void TestMyCode::runTMC() {
    TMCRunner *runner = TMCRunner::instance();
    runner->runOnActiveProject();
}

void TestMyCode::on_login_cancelbutton_clicked()
{
    loginWidget->close();
}

void TestMyCode::on_login_loginbutton_clicked()
{
    QString username = login->usernameinput->text();
    QString password = login->passwordinput->text();
    bool savePassword = login->savepasswordbox->isChecked();
    tmcClient.authenticate(username, password, savePassword);
}

void TestMyCodePlugin::Internal::TestMyCode::on_download_cancelbutton_clicked()
{
    downloadWidget->close();
}

void TestMyCodePlugin::Internal::TestMyCode::on_download_okbutton_clicked()
{
    // TODO: Download selected items from the menu

}

} // namespace Internal
} // namespace TestMyCodePlugin
