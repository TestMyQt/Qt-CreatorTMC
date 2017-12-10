#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include "ui_loginscreen.h"
#include "tmcclient.h"

#include <QString>
#include <QWidget>
#include <QLineEdit>

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(TmcClient *client, QWidget *parent = nullptr);

    void loadQSettings();

signals:

public slots:

private:
    Ui::loginform *loginWindow;
    TmcClient *m_client;

    QLineEdit *m_username;
    QLineEdit *m_password;
    QLineEdit *m_server;

    void handleLoginResponse(QString accessToken);
    void handleAuthResponse(QString clientId, QString clientSecret);
    void onLoginClicked();
    void onChangeServerClicked();
};

#endif // LOGINWIDGET_H
