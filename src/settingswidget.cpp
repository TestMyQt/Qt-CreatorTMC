#include "settingswidget.h"
#include "testmycodeconstants.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <algorithm>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    m_client = TmcClient::instance();
    settingsWindow = new Ui::settingsForm;
    settingsWindow->setupUi(this);

    m_orgComboBox = settingsWindow->orgComboBox;
    m_courseComboBox = settingsWindow->courseComboBox;
    m_workingDir = settingsWindow->workingDir;
    m_autoUpdateInterval = settingsWindow->updateInterval;
    m_userLoggedInLabel = settingsWindow->userLoggedInLabel;
}

QString SettingsWidget::getSettingsPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) % "/TestMyQt/TMC.ini";
}

void SettingsWidget::loadSettings()
{
    // Use Ini format on all platforms
    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    m_username = settings.value("username", "").toString();
    m_serverAddress = settings.value("server", TestMyCodePlugin::Constants::DEFAULT_TMC_SERVER).toString();

    m_client->setAccessToken(settings.value("accessToken", "").toString());
    m_client->setClientId(settings.value("clientId", "").toString());
    m_client->setClientSecret(settings.value("clientSecret", "").toString());
    m_client->setServerAddress(m_serverAddress);

    // Get client id and secret from the server
    m_client->authorize();

    connect(m_client, &TmcClient::authorizationFinished, this, &SettingsWidget::handleAuthResponse);
    connect(m_client, &TmcClient::authenticationFinished, this, &SettingsWidget::handleLoginResponse);

    connect(m_client, &TmcClient::organizationListReady, this, &SettingsWidget::handleOrganizationList);
    connect(m_client, &TmcClient::courseListReady, this, &SettingsWidget::handleCourseList);

    m_activeOrganization = Organization::fromQSettings(&settings);
    m_activeCourse = Course::fromQSettings(&settings);
    m_activeCourse.exerciseListFromQSettings(&settings);

    if (m_activeCourse) {
        emit activeCourseChanged(&m_activeCourse);
    }

    workingDirectory = settings.value("workingDir", "").toString();
    m_workingDir->setText(workingDirectory);
    m_interval = settings.value("autoupdateInterval", 60).toInt();
    m_autoUpdateInterval->setValue(m_interval);
    m_userLoggedInLabel->setText("Logged in as <strong>" + m_username + "</strong>");

    settings.deleteLater();

    connect(settingsWindow->logoutButton, &QPushButton::clicked, this, [=](){
        clearCredentials();
        close();
    });
    connect(settingsWindow->cancelButton, &QPushButton::clicked, this, [=](){
        setComboboxIndex(m_orgComboBox, m_activeOrganization.getName());
        setComboboxIndex(m_courseComboBox, m_activeCourse.getName());
        close();
    });

    connect(settingsWindow->okButton, &QPushButton::clicked, this, &SettingsWidget::onSettingsOkClicked);
    connect(settingsWindow->browseButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseClicked);
    connect(m_orgComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [=](const int index) {
        Organization selectedOrg = m_orgComboBox->itemData(index).value<Organization>();
        if (!selectedOrg.getName().isEmpty()) {
            m_client->getCourseList(selectedOrg);
        } else {
            m_courseComboBox->clear();
        }
    });

    // Initialize login window
    m_loginWidget = new LoginWidget();
    m_loginWidget->setFields(m_username, m_serverAddress);

    connect(m_loginWidget, &LoginWidget::credentialsChanged, this, [=](QString username, QString password) {
        m_client->authenticate(username, password);
        m_username = username;
        m_userLoggedInLabel->setText("Logged in as <strong>" + m_username + "</strong>");

        m_activeCourse = Course();
        m_activeOrganization = Organization();

        emit activeCourseChanged(&m_activeCourse);

        QSettings settings(getSettingsPath(), QSettings::IniFormat);
        settings.setValue("username", username);
        settings.deleteLater();
    });

    connect(m_loginWidget, &LoginWidget::serverAddressChanged, this, [=](QString serverAddress) {
        m_serverAddress = serverAddress;
        m_client->setServerAddress(serverAddress);
        m_client->setAccessToken("");
        m_client->authorize();

        m_activeCourse = Course();
        m_activeOrganization = Organization();

        emit activeCourseChanged(&m_activeCourse);

        QSettings settings(getSettingsPath(), QSettings::IniFormat);
        settings.setValue("server", serverAddress);
        settings.deleteLater();
    });


    connect(m_client, &TmcClient::authenticationFinished, m_loginWidget, &LoginWidget::handleLoginResponse);
    connect(m_client, &TmcClient::accessTokenNotValid, m_loginWidget, &LoginWidget::show);
}

void SettingsWidget::setUpdateInterval(int interval)
{
    m_interval = interval;
    emit autoUpdateIntervalChanged(m_interval);
}

void SettingsWidget::display()
{
    if (!m_client->isAuthenticated()) {
        showLoginWidget();
        return;
    }

    m_client->getOrganizationList();
    if (!m_activeOrganization.getName().isEmpty()) {
        m_client->getCourseList(m_activeOrganization);
    }
    m_workingDir->setText(workingDirectory);
    show();
}

void SettingsWidget::saveExercise(Exercise &ex, Course *course)
{
    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    ex.saveQSettings(&settings, course->getName());
    settings.deleteLater();
}

QString SettingsWidget::getServerAddress()
{
    return m_serverAddress;
}

void SettingsWidget::showLoginWidget()
{
    m_loginWidget->show();
}

void SettingsWidget::onBrowseClicked()
{
    QString dir = askSaveLocation();
    m_workingDir->setText(dir);
}

QString SettingsWidget::getWorkingDirectory()
{
    return workingDirectory;
}

Course* SettingsWidget::getActiveCourse()
{
    return &m_activeCourse;
}

void SettingsWidget::setComboboxIndex(QComboBox *box, QString value)
{
    int index = box->findText(value);
    if (index != -1) {
        box->setCurrentIndex(index);
    }
}

int SettingsWidget::getAutoupdateInterval()
{
    return m_interval;
}

void SettingsWidget::clearCredentials()
{
    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    settings.setValue("username", "");
    settings.setValue("accessToken", "");
    settings.deleteLater();
    m_client->setAccessToken("");
    emit enableDownloadSubmit(false);
    emit activeCourseChanged(nullptr);
}

QString SettingsWidget::askSaveLocation()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    if (!dialog.exec())
        return QString();

    QString directory = dialog.selectedFiles().at(0);
    return directory;
}

void SettingsWidget::handleLoginResponse(QString accessToken)
{
    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    if (accessToken == "") {
        settings.setValue("username", "");
    } else {
        settings.setValue("accessToken", accessToken);
        emit enableDownloadSubmit(true);
        display();
    }
    settings.deleteLater();
}

void SettingsWidget::handleAuthResponse(QString clientId, QString clientSecret)
{
    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    settings.setValue("clientId", clientId);
    settings.setValue("clientSecret", clientSecret);
    settings.deleteLater();

    m_client->setClientId(clientId);
    m_client->setClientSecret(clientSecret);
}

void SettingsWidget::handleCourseList(Organization org)
{
    m_courseComboBox->clear();
    QList<Course> courses = org.getCourses();
    std::sort(courses.begin(), courses.end(), [](const Course& lhs, const Course& rhs)
    {
        return lhs.getTitle() < rhs.getTitle();
    });

    foreach (Course c, courses) {
        m_courseComboBox->addItem(c.getTitle(), QVariant::fromValue(c));
    }
    setComboboxIndex(m_courseComboBox, m_activeCourse.getTitle());
}

void SettingsWidget::handleOrganizationList(QList<Organization> orgs)
{
    m_organizations = orgs;

    std::sort(m_organizations.begin(), m_organizations.end(), [](const Organization& lhs, const Organization& rhs)
    {
        return lhs.getName() < rhs.getName();
    });

    m_orgComboBox->clear();

    bool hasActiveOrg = !!m_activeOrganization;
    // Add "empty" org if we have none selected
    if (!hasActiveOrg) {
        m_orgComboBox->addItem(QString("Select an organization"),
                               QVariant::fromValue(Organization()));
    }

    foreach (Organization org, m_organizations) {
        m_orgComboBox->addItem(org.getName(), QVariant::fromValue(org));
    }

    if (hasActiveOrg) {
        setComboboxIndex(m_orgComboBox, m_activeOrganization.getName());
    } else {
        m_orgComboBox->setCurrentIndex(0);
        m_courseComboBox->clear();
    }
}

void SettingsWidget::onSettingsOkClicked()
{
    if (m_workingDir->text() == "") {
        QMessageBox::critical(this, "TMC", "Please set the working directory!", QMessageBox::Ok);
        return;
    }

    QSettings settings(getSettingsPath(), QSettings::IniFormat);
    QString setDir = m_workingDir->text();
    if (setDir != workingDirectory) {
        workingDirectory = setDir;
        settings.setValue("workingDir", workingDirectory);
        emit workingDirectoryChanged(workingDirectory);
    }

    int setInterval = m_autoUpdateInterval->value();
    if (setInterval != m_interval) {
        m_interval = setInterval;
        settings.setValue("autoupdateInterval", m_interval);
        emit autoUpdateIntervalChanged(m_interval);
    }

    Organization setOrg = m_orgComboBox->currentData().value<Organization>();
    if (m_activeOrganization != setOrg) {
        m_activeOrganization = setOrg;
        Organization::toQSettings(&settings, setOrg);
        emit organizationChanged(setOrg);
    }

    Course setCourse = m_courseComboBox->currentData().value<Course>();
    if (!!setCourse && m_activeCourse != setCourse) {
        m_activeCourse = setCourse;
        Course::toQSettings(&settings, setCourse);
        emit activeCourseChanged(&m_activeCourse);
    }
    settings.deleteLater();
    close();
}
