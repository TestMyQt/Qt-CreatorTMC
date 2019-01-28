#include "loginwidget.h"
#include "testmycodeconstants.h"

#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    loginWindow = new Ui::loginform;
    loginWindow->setupUi(this);

    connect(loginWindow->cancelbutton, &QPushButton::clicked, this, &LoginWidget::close);
    connect(loginWindow->loginbutton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(loginWindow->serverButton, &QPushButton::clicked, this, &LoginWidget::onChangeServerClicked);
    // Login when enter is pressed in passwordinput
    connect(loginWindow->passwordinput, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);

    m_username = loginWindow->usernameinput;
    m_password = loginWindow->passwordinput;
    m_server = loginWindow->serverInput;
}

void LoginWidget::setFields(QString username, QString server)
{
    m_username->setText(username);
    m_server->setText(server);
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
    m_password->setText("");
    Q_EMIT credentialsChanged(username, password);
}

void LoginWidget::onChangeServerClicked()
{
    QInputDialog changeAddress;
    QString address = changeAddress.getText(this, "TMC", "Set TMC Server address:",
                                             QLineEdit::Normal, m_server->text());

    QUrl userUrl = QUrl::fromUserInput(address);
    if (userUrl.scheme() != "https") {
        QMessageBox::critical(this, "TMC", "Use https:// scheme.", QMessageBox::Ok);
        return;
    }

    m_server->setText(address);
    Q_EMIT serverAddressChanged(address);
}
