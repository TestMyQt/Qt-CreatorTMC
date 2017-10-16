#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"

#include <ui_loginscreen.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QApplication>
#include <QDebug>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/shellcommand.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>

#include <utils/fileutils.h>
#include <utils/hostosinfo.h>
#include <utils/environment.h>
#include <utils/shellcommand.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QFileInfo>

#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>

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
    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));

    connect(loginAction, &QAction::triggered, this, &TestMyCode::showLoginWidget);
    connect(tmcAction, &QAction::triggered, this, &TestMyCode::runOnActiveProject);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);

    menu->menu()->setTitle(tr("TestMyCode"));
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(loginCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = Core::ActionManager::actionContainer(Core::Constants::M_WINDOW);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    // Initialize login window
    loginWidget = new QWidget;
    login = new Ui::loginform;
    login->setupUi(loginWidget);
    // Signal-Slot for login window
    QObject::connect(login->cancelbutton, SIGNAL(clicked(bool)), this, SLOT(on_cancelbutton_clicked()));
    QObject::connect(login->loginbutton, SIGNAL(clicked(bool)), this, SLOT(on_loginbutton_clicked()));
    connect(&tmcClient, &TmcClient::loginFinished, this, &TestMyCode::on_cancelbutton_clicked);
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

void TestMyCode::runOnActiveProject()
{
    ProjectExplorer::Project *project = ProjectExplorer::SessionManager::startupProject();
    if (!project) {
        QMessageBox::information(Core::ICore::mainWindow(), tr("No project"), tr("No active project"));
        return;
    }
    QString path = project->projectFilePath().parentDir().toString();
    launchTmcCLI(path);
}

void TestMyCode::launchTmcCLI(const QString &workingDirectory)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString tmc_cli = env.value("TMC_CLI", "/opt/tmc_cli.jar");

    QString testOutput = workingDirectory + "/out.txt";
    QStringList arguments;
    arguments << "-jar" << tmc_cli;
    arguments << "run-tests";
    arguments << "--exercisePath" << workingDirectory;
    arguments << "--outputPath" << testOutput;
    QMessageBox::information(Core::ICore::mainWindow(), tr("launching"), tr("%1").arg(arguments.join(QString(" "))));
    // TODO: make work in windows
    const Utils::FileName java = Utils::FileName().fromString("/usr/bin/java");

    Core::ShellCommand command(workingDirectory, env);

    Utils::SynchronousProcessResponse response
         = command.runCommand(java, arguments, 1500, workingDirectory, Utils::defaultExitCodeInterpreter);

    QMessageBox::information(Core::ICore::mainWindow(), tr("TMC CLI output"), tr("%1").arg(response.allOutput()));

    QString output = readTMCOutput(testOutput);
}

/*
   TMC CLI output format is JSON,
   where compiler / stacktrace output is in integers representing characters:

  {
    "status": "COMPILE_FAILED",
    "testResults": [],
    "logs": {
      "compiler_output": [
        [109,97,107,101,58,32,42,42,42,32,78 ...]
    }
  }

*/
QString TestMyCode::readTMCOutput(const QString &testOutput)
{
    QFile file(testOutput);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QByteArray rawData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));

    QJsonObject json = doc.object();
    const QString status = json["status"].toString();
    const QJsonObject logs = json["logs"].toObject();

    // Convert integers back to a string
    const QJsonArray compiler_output = logs["compiler_output"].toArray();
    QString compiler_output_str;
    foreach (QJsonValue character, compiler_output) {
        compiler_output_str.append(character.toInt());
    }

    file.close();

    QMessageBox::information(Core::ICore::mainWindow(), tr("%1").arg(status), tr("%1").arg(compiler_output_str));

    return compiler_output_str;
}

void TestMyCode::on_cancelbutton_clicked()
{
    loginWidget->close();
}

void TestMyCode::on_loginbutton_clicked()
{
    // TODO: Authentication
    QString username = login->usernameinput->text();
    QString password = login->passwordinput->text();
    bool savePassword = login->savepasswordbox->isChecked();
    tmcClient.authenticate(username, password, savePassword);
}

} // namespace Internal
} // namespace TestMyCodePlugin
