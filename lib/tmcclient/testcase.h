#ifndef TESTCASE_H
#define TESTCASE_H

#include <QString>
#include <QJsonObject>

class TestCase
{
public:
    TestCase(QString d_msg, QString ex, QString msg, QString name, bool pass);

public:
    QString detailed_message;
    QString exception;
    QString message;
    QString name;
    bool successful;

    static TestCase fromJson(const QJsonObject jsonCase);
};

#endif // TESTCASE_H
