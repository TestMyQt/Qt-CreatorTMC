#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "tmcclient.h"
#include <ui_settings.h>
#include "loginwidget.h"

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>

class SettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

    void loadSettings();
    void display();

    QString getWorkingDirectory();
    Course* getActiveCourse();
    int getAutoupdateInterval();

signals:
    void workingDirectoryChanged(QString location);
    void organizationChanged(Organization org);
    void activeCourseChanged(Course *course);
    void autoUpdateIntervalChanged(int interval);
    void enableDownloadSubmit(bool enable);

public slots:
    void showLoginWidget();

private:
    TmcClient *m_client;
    Ui::settingsForm *settingsWindow;

    LoginWidget *m_loginWidget;

    QComboBox *m_orgComboBox;
    QComboBox *m_courseComboBox;
    QLineEdit *m_workingDir;
    QSpinBox *m_autoUpdateInterval;
    QLabel *m_userLoggedInLabel;

    QString m_username;
    QString m_serverAddress;

    QList<Organization> m_organizations;
    Organization m_activeOrganization;
    Course m_activeCourse;
    QString workingDirectory;
    int m_interval;

    void handleLoginResponse(QString accessToken);
    void handleAuthResponse(QString clientId, QString clientSecret);
    void handleOrganizationList(QList<Organization> orgs);
    void handleCourseList(Organization org);
    void setComboboxIndex(QComboBox *box, QString value);
    void onSettingsOkClicked();
    void onBrowseClicked();
    void clearCredentials();
    void setUpdateInterval(int interval);

    QString askSaveLocation();

};

#endif // SETTINGSWIDGET_H
