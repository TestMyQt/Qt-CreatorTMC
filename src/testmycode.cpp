#include "testmycode.h"
#include "testmycodeconstants.h"
#include "tmcclient.h"
#include "tmcoutputpane.h"
#include "tmcrunner.h"
#include "course.h"

#include <ui_loginscreen.h>
#include <ui_downloadscreen.h>
#include <ui_settings.h>

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
#include <QFileDialog>
#include <QByteArray>

#include <QSettings>
#include <QList>
#include <QMap>

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

    auto logoutAction = new QAction(tr("Logout"), this);
    Core::Command *logoutCmd = Core::ActionManager::registerAction(logoutAction, Constants::LOGOUT_ACTION_ID,
                                                                     Core::Context(Core::Constants::C_GLOBAL));
    // Shortcut
    tmcCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+T")));
    loginCmd->setDefaultKeySequence(QKeySequence(tr("Alt+L")));
    settingsCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+S")));
    downloadCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+D")));
    logoutCmd->setDefaultKeySequence(QKeySequence(tr("Alt+Shift+L")));

    // Connect to trigger to a function
    connect(loginAction, &QAction::triggered, this, &TestMyCode::showLoginWidget);
    connect(tmcAction, &QAction::triggered, this, &TestMyCode::runTMC);
    connect(settingsAction, &QAction::triggered, this, &TestMyCode::showSettingsWindow);
    connect(downloadAction, &QAction::triggered, this, &TestMyCode::showDownloadWidget);
    connect(logoutAction, &QAction::triggered, this, &TestMyCode::clearCredentials);

    // Create context menu with actions
    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("TestMyCode"));
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    menu->addAction(tmcCmd);
    menu->addAction(settingsCmd);
    menu->addAction(loginCmd);
    menu->addAction(downloadCmd);
    menu->addAction(logoutCmd);

    // Add TestMyCode between Tools and Window in the upper menu
    auto tools_menu = Core::ActionManager::actionContainer(Core::Constants::M_WINDOW);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(tools_menu, menu);

    addAutoReleasedObject(TmcOutputPane::instance());

    // Initialize settings window
    settingsWidget = new QWidget;
    settingsWindow = new Ui::settingsForm;
    settingsWindow->setupUi(settingsWidget);

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
    tmcClient.setAccessToken(settings.value("accessToken", "").toString());
    tmcClient.setClientId(settings.value("clientId", "").toString());
    tmcClient.setClientSecret(settings.value("clientSecret", "").toString());
    tmcClient.setServerAddress(settings.value("server", "").toString());
    login->usernameinput->setText(settings.value("username", "").toString());
    login->serverInput->setText(settings.value("server", "").toString());
    settings.deleteLater();

    // Signal-Slot to popup login window in case of non-valid access token
    connect(&tmcClient, &TmcClient::accessTokenNotValid, this, &TestMyCode::showLoginWidget);

    // Signal-Slot for login window
    connect(login->cancelbutton, &QPushButton::clicked, this, &TestMyCode::onLoginCancelClicked);
    connect(login->loginbutton, &QPushButton::clicked, this, &TestMyCode::onLoginClicked);

    // Signal-Slot for Settings window
    connect(settingsWindow->refreshlistbutton, &QPushButton::clicked, this, &TestMyCode::onRefreshCourseListClicked);
    connect(settingsWindow->okButton, &QPushButton::clicked, this, &TestMyCode::onSettingsOkClicked);
    connect(settingsWindow->cancelButton, &QPushButton::clicked, this, [=](){
        settingsWidget->close();
    });
    connect(settingsWindow->browseButton, &QPushButton::clicked, this, &TestMyCode::onSettingsBrowseClicked);

    // TmcClient
    connect(&tmcClient, &TmcClient::authorizationFinished, this, &TestMyCode::handleAuthResponse);
    connect(&tmcClient, &TmcClient::authenticationFinished, this, &TestMyCode::handleLoginResponse);
    connect(&tmcClient, &TmcClient::exerciseListReady, this, &TestMyCode::refreshDownloadList);
    connect(&tmcClient, &TmcClient::exerciseZipReady, this, &TestMyCode::openProject);
    connect(&tmcClient, &TmcClient::TMCError, this, &TestMyCode::displayTMCError);
    connect(&tmcClient, &TmcClient::organizationListReady, this, &TestMyCode::handleOrganizationList);
    connect(&tmcClient, &TmcClient::courseListReady, this, &TestMyCode::handleCourseList);

    // Signal-Slot for download window
    connect(downloadform->cancelbutton, &QPushButton::clicked, this, &TestMyCode::onDownloadCancelClicked);
    connect(downloadform->okbutton, &QPushButton::clicked, this, &TestMyCode::onDownloadOkClicked);
    connect(&tmcClient, &TmcClient::closeDownloadWindow, this, [=](){
        downloadWidget->close();
    });

    QNetworkAccessManager *m = new QNetworkAccessManager;
    tmcClient.setNetworkManager(m);

    // TODO: authorization happens at plugin creation time, but this could be done
    // when a server is activated in preferences
    if (!tmcClient.isAuthorized()) {
        tmcClient.authorize();
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

void TestMyCode::showLoginWidget()
{
    loginWidget->show();
}

void TestMyCode::showDownloadWidget()
{
    setDefaultCourse();
    downloadWidget->show();
}

void TestMyCode::showSettingsWidget()
{
    // TODO getOrganizationList
    // also implement currentTextChanged signal-slot for org list
    QSettings settings("TestMyQt", "TMC");

    settingsWindow->workingDir->setText(settings.value("workingDir", "").toString());

    settings.deleteLater();
    settingsWidget->show();
}

void TestMyCode::runTMC()
{
    TMCRunner *runner = TMCRunner::instance();
    runner->runOnActiveProject();
}

void TestMyCode::refreshDownloadList()
{
    QList<Exercise> * exercises = m_activeCourse->getExercises();
    // Create item on-the-run
    downloadform->exerciselist->clear();
    for(int i = 0; i < exercises->count(); i++) {
        QListWidgetItem* item = new QListWidgetItem(exercises->at(i).getName(), downloadform->exerciselist);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Checked);
    }
}

void TestMyCode::openProject(Exercise *ex)
{
    Q_UNUSED(ex)
}

void TestMyCode::displayTMCError(QString errorText)
{
    QMessageBox::critical(nullptr, "TMC", errorText, QMessageBox::Ok);
}

void TestMyCode::setDefaultCourse()
{
    tmcClient.getUserInfo();
    Course* course = new Course();
    course->setId(18);
    m_activeCourse = course;
    tmcClient.getExerciseList(course);
}

void TestMyCode::onLoginCancelClicked()
{
    loginWidget->close();
}

void TestMyCode::onLoginClicked()
{
    QString username = login->usernameinput->text();
    QString password = login->passwordinput->text();
    QString serverAddress = login->serverInput->text();
    tmcClient.setServerAddress(serverAddress);
    tmcClient.authenticate(username, password);
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", username);
    settings.setValue("server", serverAddress);
    settings.deleteLater();
}

void TestMyCode::handleAuthResponse(QString clientId, QString clientSecret)
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("clientId", clientId);
    settings.setValue("clientSecret", clientSecret);
    settings.deleteLater();
}

void TestMyCode::handleLoginResponse(QString accessToken)
{
    QSettings settings("TestMyQt", "TMC");
    // We did not receive any token, credentials were not valid
    if (accessToken == "") {
        settings.setValue("username", "");
        settings.setValue("password", "");
    } else {
        settings.setValue("accessToken", accessToken);
        loginWidget->close();
    }

    settings.deleteLater();
}

void TestMyCode::clearCredentials()
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", "");
    settings.setValue("accessToken", "");
    settings.deleteLater();
    tmcClient.setAccessToken("");
}

void TestMyCode::handleOrganizationList(QMap<QString, QString> organizations)
{
    settingsWindow->orgComboBox->clear();
    QMapIterator<QString, QString> iter(organizations);
    while(iter.hasNext()) {
        iter.next();
        settingsWindow->orgComboBox->addItem(iter.key(), iter.value());
    }
    // TODO set text to currently selected organziation,
    // if that is found also trigger getCourseList
}

void TestMyCode::handleCourseList(QMap<QString, int> courses)
{
    settingsWindow->coursecombobox->clear();
    QMapIterator<QString, int> iter(courses);
    while(iter.hasNext()) {
        iter.next();
        settingsWindow->coursecombobox->addItem(iter.key(), iter.value());
    }
    // TODO set text to currently selected course if it exists
}

void TestMyCode::onSettingsOkClicked()
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("workingDir", settingsWindow->workingDir->text());
    settings.setValue("orgName", settingsWindow->orgComboBox->currentText();
    settings.setValue("orgSlug", settingsWindow->orgComboBox->currentData());
    settings.setValue("courseName", settingsWindow->courseComboBox->currentText());
    settings.setValue("courseId", settingsWindow->courseComboBox->currentData());
    settings.deleteLater();
    settingsWidget->close();
}

void TestMyCode::onSettingsBrowseClicked()
{
    QFileDialog dialog(downloadWidget);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    QString dir = dialog.selectedFiles().at(0);
    settingsWindow->workingDir->setText(dir);
}

void TestMyCode::onDownloadCancelClicked()
{
    downloadWidget->close();
}

void TestMyCode::onDownloadOkClicked()
{
    auto exerciseList = downloadform->exerciselist;
    qDebug() << "There are " << exerciseList->count() << "exercises to be loaded.";

    QSettings settings("TMC", "TestMyQt");
    QString workingDir = settings.value("workingDir", "").toString();
    QString courseName = settings.value("courseName", "").toString();
    if (workingDir == "" || courseName == "")
        return;
    QString saveDirectory = workingDir + "/" + courseName;
    settings.deleteLater();

    downloadPanel = new DownloadPanel();

    for (int idx = 0; idx < exerciseList->count(); idx++) {
        if (exerciseList->item(idx)->checkState() == Qt::Checked)
        {
            qDebug() << "Downloading exercise" << exerciseList->item(idx)->text();
            Exercise *ex = &((*m_activeCourse->getExercises())[idx]);
            ex->setLocation(saveDirectory);
            downloadPanel->addWidgetsToDownloadPanel( ex->getName() );
            QNetworkReply* reply = tmcClient.getExerciseZip(ex);

            connect( reply, &QNetworkReply::downloadProgress,
                downloadPanel, &DownloadPanel::networkReplyProgress );
            connect( reply, &QNetworkReply::finished,
                downloadPanel, &DownloadPanel::httpFinished );

            downloadPanel->addReplyToList( reply );
        }
    }

    downloadPanel->addInfoLabel();
    downloadPanel->sanityCheck(); // Should be removed at some point
    downloadPanel->show();
}

} // namespace Internal
} // namespace TestMyCodePlugin
