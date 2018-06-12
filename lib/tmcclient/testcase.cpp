#include "testcase.h"

TestCase::TestCase(QString d_msg, QString ex, QString msg, QString name, bool pass) :
    detailed_message(d_msg),
    exception(ex),
    message(msg),
    name(name),
    successful(pass)
{

}

TestCase TestCase::fromJson(const QJsonObject jsonCase)
{
    return TestCase(
                jsonCase["detailed_message"].toString(),
            jsonCase["exception"].toString(),
            jsonCase["message"].toString(),
            jsonCase["name"].toString(),
            jsonCase["successful"].toBool());
}
