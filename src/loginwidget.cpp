#include "loginwidget.h"

#include <QPushButton>
#include <QInputDialog>

LoginWidget::LoginWidget(TmcClient *client, QWidget *parent) :
    QWidget(parent),
    m_client(client)
{
    connect(m_client, &TmcClient::authenticationFinished, this, &LoginWidget::handleLoginResponse);
    connect(m_client, &TmcClient::accessTokenNotValid, this, &LoginWidget::show);

    loginWindow = new Ui::loginform;
    loginWindow->setupUi(this);

    connect(loginWindow->cancelbutton, &QPushButton::clicked, this, &LoginWidget::close);
    connect(loginWindow->loginbutton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(loginWindow->serverButton, &QPushButton::clicked, this, &LoginWidget::onChangeServerClicked);

    m_username = loginWindow->usernameinput;
    m_password = loginWindow->passwordinput;
    m_server = loginWindow->serverInput;
}

void LoginWidget::loadQSettings()
{
    QSettings settings("TestMyQt", "TMC");
    m_username->setText(settings.value("username", "").toString());
    m_server->setText(settings.value("server", "").toString());
    settings.deleteLater();
}

void LoginWidget::handleLoginResponse(QString accessToken)
{
    if (accessToken != "") {
        close();
    }
    // TODO: Indicate login failure visually
}

void LoginWidget::onLoginClicked()
{
    QString username = m_username->text();
    QString password = m_password->text();
    m_client->authenticate(username, password);
    QSettings settings("TestMyQt", "TMC");
    settings.setValue("username", username);
    settings.setValue("server", m_server->text());
    settings.deleteLater();
}

void LoginWidget::onChangeServerClicked()
{
    QInputDialog changeAddress;
    QString address = changeAddress.getText(this, "TMC", "Set TMC Server address:",
                                             QLineEdit::Normal, m_server->text());
    if (!address.isEmpty()) {
        m_server->setText(address);
        m_client->setServerAddress(address);
        m_client->authorize();
    }
}
