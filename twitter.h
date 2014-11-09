#ifndef TWITTER_H
#define TWITTER_H

#include <QObject>
#include <QNetworkAccessManager>

#include "settings.h"

class Twitter : public QObject
{
    Q_OBJECT
public:
    explicit Twitter(QObject *parent = 0);

    QString getScreenName();
    QString getName();
    void statusUpdate(QString text, QString in_reply_to_status_id = NULL);
    void updateName(QString name);

private:
    QByteArray requestTwitterApi(const QNetworkAccessManager::Operation httpMethod,
                                 const QString url,
                                 const QVariantMap dataParams);

    Settings settings;
    const QString ACCOUNT_VERIFY_CREDENTIALS_URL = QString("https://api.twitter.com/1.1/account/verify_credentials.json");
    const QString ACCOUNT_UPDATE_PROFILE_URL     = QString("https://api.twitter.com/1.1/account/update_profile.json");
    const QString STATUSES_UPDATE_URL            = QString("https://api.twitter.com/1.1/statuses/update.json");
};

#endif // TWITTER_H
