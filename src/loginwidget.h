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
    explicit LoginWidget(QWidget *parent = nullptr);

    void setFields(QString username, QString server);

Q_SIGNALS:
    void credentialsChanged(QString username, QString password);
    void serverAddressChanged(QString server);

public Q_SLOTS:
    void handleLoginResponse(QString accessToken);

private:
    Ui::loginform *loginWindow;

    QLineEdit *m_username;
    QLineEdit *m_password;
    QLineEdit *m_server;

    void onLoginClicked();
    void onChangeServerClicked();
};

#endif // LOGINWIDGET_H
