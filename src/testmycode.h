#pragma once
#include "testmycode_global.h"
#include "tmcclient.h"
#include "settingswidget.h"
#include "tmcmanager.h"

#include <extensionsystem/iplugin.h>

#include <QObject>
#include <QString>

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

private:
    // tmcClient
    TmcClient *tmcClient;

    // SettingsWidget
    SettingsWidget *m_settingsWidget;

    // TmcManager
    TmcManager *m_tmcManager;

    void displayTMCError(QString errorText);
};

} // namespace Internal
} // namespace TestMyCodePlugin
