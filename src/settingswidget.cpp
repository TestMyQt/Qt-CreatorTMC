#include "settingswidget.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    settingsWindow = new Ui::settingsForm;
    settingsWindow->setupUi(this);

    // Initialize login window
    loginWidget = new LoginWidget;

    m_orgComboBox = settingsWindow->orgComboBox;
    m_courseComboBox = settingsWindow->courseComboBox;
    m_workingDir = settingsWindow->workingDir;

    QSettings settings("TestMyQt", "TMC");
    m_activeOrganization = Organization::fromQSettings(&settings);
    m_activeCourse = Course::fromQSettings(&settings);
    workingDirectory = settings.value("workingDir", "").toString();
    m_autoUpdateInterval = settings.value("autoupdateInterval", 60).toInt();

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
    connect(m_orgComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index){
        m_client->getCourseList(m_orgComboBox->itemData(index).value<Organization>());
    });
}

void SettingsWidget::setTmcClient(TmcClient *client)
{
    m_client = client;

    QSettings settings("TestMyQt", "TMC");
    m_client->setAccessToken(settings.value("accessToken", "").toString());
    m_client->setClientId(settings.value("clientId", "").toString());
    m_client->setClientSecret(settings.value("clientSecret", "").toString());
    m_client->setServerAddress(settings.value("server", "").toString());
    settings.deleteLater();

    loginWidget->setTmcClient(m_client);


    connect(m_client, &TmcClient::authorizationFinished, this, &SettingsWidget::handleAuthResponse);
    connect(m_client, &TmcClient::authenticationFinished, this, &SettingsWidget::handleLoginResponse);

    connect(m_client, &TmcClient::organizationListReady, this, &SettingsWidget::handleOrganizationList);
    connect(m_client, &TmcClient::courseListReady, this, &SettingsWidget::handleCourseList);
}

void SettingsWidget::setUpdateInterval(int interval)
{
    m_autoUpdateInterval = interval;
    emit autoUpdateIntervalChanged(m_autoUpdateInterval);
}

void SettingsWidget::display()
{
    m_client->getOrganizationList();
    if (!m_activeOrganization.getName().isEmpty()) {
        m_client->getCourseList(m_activeOrganization);
    }
    m_workingDir->setText(workingDirectory);
    show();
}

void SettingsWidget::showLoginWidget()
{
    loginWidget->loadQSettings();
    loginWidget->show();
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
    return m_autoUpdateInterval;
}

void SettingsWidget::onBrowseClicked()
{
    QString dir = askSaveLocation();
    m_workingDir->setText(dir);
}

void SettingsWidget::clearCredentials()
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", "");
    settings.setValue("accessToken", "");
    settings.deleteLater();
    m_client->setAccessToken("");
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
    QSettings settings("TestMyQt", "TMC");
    if (accessToken == "") {
        settings.setValue("username", "");
    } else {
        settings.setValue("accessToken", accessToken);
        display();
    }
    settings.deleteLater();
}

void SettingsWidget::handleAuthResponse(QString clientId, QString clientSecret)
{
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("clientId", clientId);
    settings.setValue("clientSecret", clientSecret);
    settings.deleteLater();
}

void SettingsWidget::handleCourseList(Organization org)
{
    m_courseComboBox->clear();
    foreach (Course c, org.getCourses()) {
        m_courseComboBox->addItem(c.getName(), QVariant::fromValue(c));
    }
    setComboboxIndex(m_courseComboBox, m_activeCourse.getName());
}

void SettingsWidget::handleOrganizationList(QList<Organization> orgs)
{
    m_organizations = orgs;
    m_orgComboBox->clear();
    foreach (Organization org, m_organizations) {
        m_orgComboBox->addItem(org.getName(), QVariant::fromValue(org));
    }
    setComboboxIndex(m_orgComboBox, m_activeOrganization.getName());
}

void SettingsWidget::onSettingsOkClicked()
{
    if (m_workingDir->text() == "") {
        QMessageBox::critical(this, "TMC", "Please set the working directory!", QMessageBox::Ok);
        return;
    }

    QSettings settings("TestMyQt", "TMC");
    QString setDir = m_workingDir->text();
    if (setDir != workingDirectory) {
        workingDirectory = setDir;
        settings.setValue("workingDir", workingDirectory);
        emit workingDirectoryChanged(workingDirectory);
    }

    Organization setOrg = m_orgComboBox->currentData().value<Organization>();
    if (m_activeOrganization.getSlug() != setOrg.getSlug()) {
        m_activeOrganization = setOrg;
        Organization::toQSettings(&settings, setOrg);
        emit organizationChanged(setOrg);
    }

    Course setCourse = m_courseComboBox->currentData().value<Course>();
    if (m_activeCourse.getId() != setCourse.getId()) {
        m_activeCourse = setCourse;
        Course::toQSettings(&settings, setCourse);
        emit activeCourseChanged(&m_activeCourse);
    }
    settings.deleteLater();
    close();
}
