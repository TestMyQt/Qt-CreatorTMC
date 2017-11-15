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

    content += QString("{\"course\":"
                       "{ \"id\": 18,"
                       "\"name\": \"qt-course\","
                       "\"title\": \"Qt course 2017\","
                       "\"reviews_url\": \"https://tmc.mooc.fi/api/v8/core/courses/18/reviews\","
                       "\"spyware_urls\": [\"http://hy.spyware.testmycode.net/\"],"
                       "\"unlock_url\": \"https://tmc.mooc.fi/api/v8/core/courses/18/unlock\","
                       "\"unlockables\": [],"
                       "\"description\": \"Course for Qt\","
                       "\"details_url\": \"https://tmc.mooc.fi/api/v8/core/courses/18\","
                       "\"exercises\": ["
                       "{"
                        "\"name\": \"Set1-01.Exercise1\","
                        "\"all_review_points_given\": true,"
                        "\"attempted\": false,"
                        "\"checksum\": \"0c6f85e1b1b885c68079a4f9b301fb50\","
                        "\"code_review_requests_enabled\": true,"
                        "\"completed\": false,"
                        "\"deadline\": null,"
                        "\"deadline_description\": null,"
                        "\"exercise_submissions_url\": \"https://tmc.mooc.fi/api/v8/core/exercises/1538\","
                        "\"id\": 1538,"
                        "\"locked\": false,"
                        "\"memory_limit\": null,"
                        "\"requires_review\": false,"
                        "\"return_url\": \"https://tmc.mooc.fi/api/v8/core/exercises/1538/submissions\","
                        "\"returnable\": true,"
                        "\"reviewed\": false,"
                        "\"run_tests_locally_action_enabled\": true,"
                        "\"runtime_params\": [],"
                        "\"valgrind_strategy\": \"fail\","
                        "\"zip_url\": \"https://tmc.mooc.fi/api/v8/core/exercises/1538/download\""
                       "}"
                       "]"
                       "}"
                       "}");


    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusOK();
    testmanager->setReply(reply);

    qRegisterMetaType<Course*>("Course*");
    QSignalSpy exerciseList(&tmcClient, &TmcClient::exerciseListReady);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);

    Course *c = new Course();
    c->setId(18);
    c->setTitle("TestCourse");
    tmcClient.getExerciseList(c);
    reply->setContent(content);

    QVERIFY2(exerciseList.count() == 1, "exerciseListReady");
    QVERIFY2(error.count() == 0, "TMCError");

    Exercise e = c->getExercise(1538);
    QVERIFY2(e.getName() == "Set1-01.Exercise1", "name");
    QVERIFY2(e.getChecksum() == "0c6f85e1b1b885c68079a4f9b301fb50", "checksum");
}

void TmcClientTest::testSuccessfulLogin()
{
    content += QString("{ \"access_token\": \"token\",");
    content += QString("\"created_at\": 1510207179,");
    content += QString("\"scope\": \"public\",");
    content += QString("\"token_type\": \"bearer\" }");
    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusOK();
    reply->setContentType("text/json");
    testmanager->setReply(reply);
    QSignalSpy login(&tmcClient, &TmcClient::loginFinished);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);
    tmcClient.authenticate("testiCredentials", "?", false);
    reply->setContent(content);

    QVERIFY2(login.count() == 1, "loginFinished");
    QVERIFY2(error.count() == 0, "TMCError");
}

void TmcClientTest::testUnsuccessfulLogin()
{
    content += QString("{ \"access_token\": \"token\",");
    content += QString("\"created_at\": 1510207179,");
    content += QString("\"scope\": \"public\",");
    content += QString("\"token_type\": \"bearer\" }");
    TestNetworkReply *reply = new TestNetworkReply;
    reply->setStatusUnauthorized();
    reply->setContentType("text/json");
    testmanager->setReply(reply);
    QSignalSpy login(&tmcClient, &TmcClient::loginFinished);
    QSignalSpy error(&tmcClient, &TmcClient::TMCError);
    tmcClient.authenticate("testiCredentials", "?", false);
    reply->setContent(content);

    QVERIFY2(login.count() == 0, "loginFinished");
    QVERIFY2(error.count() == 1, "TMCError");
}

QTEST_MAIN(TmcClientTest)
// Include meta object compiler output because we have no header file
#include "tmcclient_tests.moc"
