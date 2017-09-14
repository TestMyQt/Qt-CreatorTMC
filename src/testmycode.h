#pragma once

#include "testmycode_global.h"

#include <extensionsystem/iplugin.h>

namespace TestMyCodePlugin {
namespace Internal {

class TestMyCodePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TestMyCode.json")

public:
    TestMyCodePlugin();
    ~TestMyCodePlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    void triggerAction();
};

} // namespace Internal
} // namespace TestMyCodePlugin
