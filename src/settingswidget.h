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
    void setTmcClient(TmcClient *client);

    void display();
    void showLoginWidget();

    QString getWorkingDirectory();
    Course* getActiveCourse();

signals:
    void workingDirectoryChanged(QString location);
    void organizationChanged(Organization org);
    void activeCourseChanged(Course *course);

public slots:

private:
    TmcClient *m_client;
    Ui::settingsForm *settingsWindow;

    // LoginWidget
    LoginWidget *loginWidget;

    QComboBox *m_orgComboBox;
    QComboBox *m_courseComboBox;
    QLineEdit *m_workingDir;

    QList<Organization> m_organizations;
    Organization m_activeOrganization;
    Course *m_activeCourse;
    QString workingDirectory;

    void handleLoginResponse(QString accessToken);
    void handleAuthResponse(QString clientId, QString clientSecret);
    void handleOrganizationList(QList<Organization> orgs);
    void handleCourseList(Organization org);
    void onSettingsOkClicked();
    void onBrowseClicked();
    void clearCredentials();

    QString askSaveLocation();

};

#endif // SETTINGSWIDGET_H
