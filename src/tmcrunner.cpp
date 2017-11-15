#include "tmcrunner.h"
#include "tmcoutputpane.h"
#include "tmctestresult.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/progressmanager/futureprogress.h>
#include <coreplugin/progressmanager/progressmanager.h>

#include <projectexplorer/projectexplorersettings.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/buildmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/session.h>

#include <utils/outputformat.h>
#include <utils/runextensions.h>
#include <utils/hostosinfo.h>
#include <utils/fileutils.h>
#include <utils/environment.h>
#include <utils/shellcommand.h>

#include <QProcessEnvironment>
#include <QFileInfo>
#include <QFuture>
#include <QFutureInterface>
#include <QTime>

#include <QMessageBox>
#include <QList>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

static TMCRunner *s_instance = nullptr;

TMCRunner *TMCRunner::instance()
{
    if (!s_instance)
        s_instance = new TMCRunner;
    return s_instance;
}

TMCRunner::TMCRunner(QObject *parent) :
    QObject(parent)
{

}

TMCRunner::~TMCRunner()
{
    s_instance = nullptr;
}

void TMCRunner::runOnActiveProject()
{
    ProjectExplorer::Project *project = ProjectExplorer::SessionManager::startupProject();
    if (!project) {
        QMessageBox::information(Core::ICore::mainWindow(), tr("No project"), tr("No active project"));
        return;
    }
    const Utils::FileName project_path = project->projectFilePath().parentDir();
    launchTmcCLI(project_path);
}

void TMCRunner::launchTmcCLI(const Utils::FileName &workingDirectory)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString tmc_cli = env.value("TMC_CLI", "/opt/tmc_cli.jar");
    QString dir = workingDirectory.toString();

    QString testOutput = dir + "/out.txt";
    QStringList arguments;
    arguments << "-jar" << tmc_cli;
    arguments << "run-tests";
    arguments << "--exercisePath" << dir;
    arguments << "--outputPath" << testOutput;
    QMessageBox::information(Core::ICore::mainWindow(), tr("launching"), tr("%1").arg(arguments.join(QString(" "))));
    // TODO: make work in windows
    const Utils::FileName java = Utils::FileName().fromString("java");

    Utils::ShellCommand command(dir, env);

    Utils::SynchronousProcessResponse response
         = command.runCommand(java, arguments, 1500, dir, Utils::defaultExitCodeInterpreter);

    QMessageBox::information(Core::ICore::mainWindow(), tr("TMC output"), tr("%1").arg(response.allOutput()));

    QList<TmcTestResult> output = readTMCOutput(testOutput);
    emit testResultReady(output);
}

/*
   TMC CLI output format is JSON,
   where compiler / stacktrace output is in integers representing characters:

  {
    "status": "COMPILE_FAILED",
    "testResults": [],
    "logs": {
      "compiler_output": [
        [109,97,107,101,58,32,42,42,42,32,78 ...]
    }
  }

*/
QList<TmcTestResult> TMCRunner::readTMCOutput(const QString &testOutput)
{
    QFile file(testOutput);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(rawData));

    const QJsonObject json = doc.object();
    const QString status = json["status"].toString();
    const QJsonObject logs = json["logs"].toObject();

    // Convert integers back to a string
    const QJsonArray compiler_output = logs["compiler_output"].toArray();
    QString compiler_output_str;
    foreach (QJsonValue character, compiler_output) {
        compiler_output_str.append(character.toInt());
    }

    QMessageBox::information(Core::ICore::mainWindow(), tr("%1").arg(status), tr("%1").arg(compiler_output_str));
    QList<TmcTestResult> results;

    const QJsonArray testResults = json["testResults"].toArray();
    foreach (QJsonValue result, testResults) {
        QJsonObject r = result.toObject();
        TmcTestResult tmcResult(r["name"].toString());
        tmcResult.setSuccess(r["successful"].toBool());
        tmcResult.setMessage(r["message"].toString());
        tmcResult.setException(r["exception"].toString());

        QList<QString> points;
        QJsonArray points_array = r["points"].toArray();
        foreach (QJsonValue point, points_array) {
            points.append(point.toString());
        }
        tmcResult.setPoints(points);
        results.append(tmcResult);
    }

    return results;
}
