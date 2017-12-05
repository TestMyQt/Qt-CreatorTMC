#include <QtTest>

#include "tmcclient.h"
#include "testnetworkaccessmanager.h"

class TmcClientTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase(); // called before the first test function is executed
    void init(); // called before each test function is executed
    void cleanup(); // called after every test function
    void cleanupTestCase(); // called after the last test function was executed

    // Declare unit test prototype definitions here
    void testCourseList();
    void testSuccessfulLogin();
    void testUnsuccessfulLogin();
    void testAuthorization();

private:
    TestNetworkAccessManager *testmanager;
    QString content;
    TmcClient tmcClient;
};

void TmcClientTest::initTestCase()
{
    // called before the first test function is executed
    testmanager = new TestNetworkAccessManager;
    tmcClient.setNetworkManager(testmanager);
}

void TmcClientTest::init() {
    // called before each test function is executed
}

void TmcClientTest::cleanup() {
    // called after every test function
}

void TmcClientTest::cleanupTestCase()
{
    // called after the last test function was executed
}

// Implement unit test functions here.

void TmcClientTest::testCourseList()
{

    QFile file;
    file.setFileName(QFINDTESTDATA("testdata/course.json"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    content = file.readAll();

    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusOK();
    testmanager->setReply(reply);

    qRegisterMetaType<Course*>("Course*");
    QSignalSpy exerciseList(&tmcClient, &TmcClient::exerciseListReady);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);

    Course *c = new Course();
    c->setId(18);
    c->setName("TestCourse");
    tmcClient.getExerciseList(c);
    reply->setContent(content);

    QVERIFY2(exerciseList.count() == 1, "exerciseListReady");
    QList<QVariant> arguments = exerciseList.takeFirst();

    Course *course = QVariant::fromValue(arguments.at(0)).value<Course*>();
    QList<Exercise> newExercises = QVariant::fromValue(arguments.at(1)).value<QList<Exercise>>();

    QVERIFY2(error.count() == 0, "TMCError");

    QVERIFY2(course->getId() == 18, "id");
    QVERIFY2(course->getName() == "TestCourse", "name");

    Exercise e = newExercises.takeFirst();
    QVERIFY2(e.getId() == 1337, "id");
    QVERIFY2(e.getName() == "Exercise1", "name");
    QVERIFY2(e.getChecksum() == "d41d8cd98f00b204e9800998ecf8427e", "checksum");

    Exercise e2 = newExercises.takeLast();
    QVERIFY2(e2.getId() == 1338, "id");
    QVERIFY2(e2.getName() == "Exercise2", "name");
    QVERIFY2(e2.getChecksum() == "d41d8cd98f00b204e9800998ecf8427f", "checksum");
}

void TmcClientTest::testSuccessfulLogin()
{
    QFile file;
    file.setFileName(QFINDTESTDATA("testdata/token.json"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    content = file.readAll();

    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusOK();
    reply->setContentType("text/json");
    testmanager->setReply(reply);
    QSignalSpy login(&tmcClient, &TmcClient::authenticationFinished);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);
    tmcClient.setClientId("clientId");
    tmcClient.setClientSecret("clientSecret");
    tmcClient.authenticate("testiCredentials", "?");
    reply->setContent(content);

    QVERIFY2(login.count() == 1, "authenticationFinished");
    QVERIFY2(error.count() == 0, "TMCError");

    QList<QVariant> arguments = login.takeFirst();
    QVERIFY2(arguments.at(0).toString() == "token", "token");
}

void TmcClientTest::testUnsuccessfulLogin()
{
    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusUnauthorized();
    reply->setContentType("text/json");
    testmanager->setReply(reply);
    QSignalSpy login(&tmcClient, &TmcClient::authenticationFinished);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);
    tmcClient.setClientId("clientId");
    tmcClient.setClientSecret("clientSecret");
    tmcClient.authenticate("testiCredentials", "?");
    reply->setContent(QString(""));

    QVERIFY2(login.count() == 1, "authenticationFinished");
    QVERIFY2(error.count() == 1, "TMCError");

    QList<QVariant> arguments = login.takeFirst();
    QVERIFY2(arguments.at(0).toString() == "", "token");
}

void TmcClientTest::testAuthorization()
{
    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusUnauthorized();
    reply->setContentType("text/json");
    testmanager->setReply(reply);
    QSignalSpy login(&tmcClient, &TmcClient::authenticationFinished);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);
    tmcClient.setClientId("");
    tmcClient.setClientSecret("");
    tmcClient.authenticate("testiCredentials", "?");
    reply->setContent(QString(""));

    QVERIFY2(login.count() == 1, "authenticationFinished");
    QVERIFY2(error.count() == 1, "TMCError");

    QList<QVariant> arguments = login.takeFirst();
    QVERIFY2(arguments.at(0).toString() == "", "token");
}

QTEST_MAIN(TmcClientTest)
// Include meta object compiler output because we have no header file
#include "tmcclient_tests.moc"
