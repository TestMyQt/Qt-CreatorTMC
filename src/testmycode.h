#pragma once

#include "testmycode_global.h"

#include <extensionsystem/iplugin.h>

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

private:
    void createLoginForm();
};

} // namespace Internal
} // namespace TestMyCodePlugin
