#ifndef TMCCLIENT_H
#define TMCCLIENT_H

#include "exercise.h"
#include "course.h"
#include "organization.h"
#include "submission.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QUrlQuery>
#include <QBuffer>

class TmcClient : public QObject
{
    Q_OBJECT
public:
    explicit TmcClient(QObject *parent = nullptr);
    static TmcClient *instance();

    void setNetworkManager(QNetworkAccessManager *m);
    void setAccessToken(const QString &token);
    void setClientId(const QString &id);
    void setClientSecret(const QString &secret);
    void setServerAddress(QString &address);

    void authorize();
    void authenticate(const QString &username, const QString &password);
    void getUserInfo();
    void getExerciseList(Course *course);
    QNetworkReply *getExerciseZip(const Exercise &ex);
    void postExerciseZip(const Exercise &ex, const QByteArray &zipData);
    void getSubmissionStatus(int submissionId);
    void getCourseList(Organization &org);
    void getOrganizationList();

    bool isAuthorized();
    bool isAuthenticated();

Q_SIGNALS:
    void TMCError(QString errorString);
    void authorizationFinished(QString clientId, QString clientSecret);
    void authenticationFinished(QString accessToken);
    void exerciseListReady(Course *course, QList<Exercise> courseList);
    void exerciseZipReady(QByteArray zipData, Exercise ex);
    void exerciseSubmitProgress(Exercise ex, qint64 bytesSent, qint64 bytesTotal);
    void exerciseSubmitReady(Exercise ex, QString submissionUrl);
    void organizationListReady(QList<Organization> organizations);
    void courseListReady(Organization organization);
    void submissionStatusReady(Submission submission);
    void accessTokenNotValid();

private Q_SLOTS:
    void authorizationReplyFinished (QNetworkReply *reply);
    void authenticationReplyFinished (QNetworkReply *reply);
    void organizationListReplyFinished(QNetworkReply *reply);
    void courseListReplyFinished(QNetworkReply *reply, Organization org);
    void exerciseListReplyFinished (QNetworkReply *reply, Course *course);
    void exerciseZipReplyFinished (QNetworkReply *reply, const Exercise &ex);
    void zipSubmitReplyFinished(QNetworkReply *reply, const Exercise &ex);
    void submissionStatusReplyFinished(QNetworkReply *reply, int submissionId);


private:
    QNetworkRequest buildRequest(const QUrl &url);
    QNetworkReply* doGet(const QUrl &url);
    QNetworkAccessManager *m_manager;
    QString m_accessToken;
    QString m_clientId;
    QString m_clientSecret;
    QString m_serverAddress;
    bool checkRequestStatus(QNetworkReply *reply);
};

#endif // TMCCLIENT_H
