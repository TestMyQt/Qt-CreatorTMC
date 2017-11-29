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

    settings.deleteLater();

    connect(settingsWindow->logoutButton, &QPushButton::clicked, this, [=](){
        clearCredentials();
        close();
    });
    connect(settingsWindow->cancelButton, &QPushButton::clicked, this, [=](){
        close();
    });

    connect(settingsWindow->okButton, &QPushButton::clicked, this, &SettingsWidget::onSettingsOkClicked);
    connect(settingsWindow->browseButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseClicked);
    connect(m_orgComboBox, &QComboBox::currentTextChanged, this, [=](){
        if (!m_orgComboBox->currentData().toString().isEmpty()) {
            m_client->getCourseList(m_organizations.at(m_orgComboBox->currentIndex()));
        }
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

void SettingsWidget::display()
{
    m_client->getOrganizationList();
    show();
}

void SettingsWidget::showLoginWidget()
{
    loginWidget->show();
}

QString SettingsWidget::getWorkingDirectory()
{
    return workingDirectory;
}

Course* SettingsWidget::getActiveCourse()
{
    return m_activeCourse;
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
        m_courseComboBox->addItem(c.getName(), c.getId());
    }
    m_courseComboBox->setCurrentText(m_activeCourse->getName());
}

void SettingsWidget::handleOrganizationList(QList<Organization> orgs)
{
    m_organizations = orgs;
    m_orgComboBox->clear();
    m_courseComboBox->clear();
    foreach (Organization org, m_organizations) {
        m_orgComboBox->addItem(org.getName(), org.getSlug());
    }
    m_orgComboBox->setCurrentText(m_activeOrganization.getName());
}

void SettingsWidget::onSettingsOkClicked()
{
    if (m_workingDir->text() == "") {
        QMessageBox::critical(this, "TMC", "Please set the working directory!", QMessageBox::Ok);
        return;
    }
    workingDirectory = m_workingDir->text();

    emit workingDirectoryChanged(workingDirectory);
    emit organizationChanged(Organization(m_orgComboBox->currentText(),
                                          m_orgComboBox->currentData().toString()));

    if (m_activeCourse->getId() != m_courseComboBox->currentData().toInt()) {
        Course *newCourse = new Course();
        newCourse->setName(m_courseComboBox->currentText());
        newCourse->setId(m_courseComboBox->currentData().toInt());
        QSettings settings("TestMyQt", "TMC");
        newCourse->exerciseListFromQSettings(&settings);
        m_activeCourse = newCourse;
        emit activeCourseChanged(m_activeCourse);
        settings.deleteLater();
    }
    m_orgComboBox->clear();
    close();
}
