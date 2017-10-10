#pragma once
#include <ui_loginscreen.h>
#include "testmycode_global.h"
#include "authentication.h"
#include <extensionsystem/iplugin.h>
#include <QWidget>

namespace TestMyCodePlugin {
namespace Internal {

class TestMyCode : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TestMyCode.json")

public:
    TestMyCode();
    ~TestMyCode();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void on_cancelbutton_clicked();
    void on_loginbutton_clicked();

private:
    // Loginform
    Ui::loginform *login;
    QWidget *loginWidget;
    void showLoginWidget();
    Authentication auth;
};

} // namespace Internal
} // namespace TestMyCodePlugin
