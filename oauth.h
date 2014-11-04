#ifndef OAUTH_H
#define OAUTH_H

#include <QObject>
#include <QUrl>

#include "settings.h"

class OAuth : public QObject
{
    Q_OBJECT
public:
    explicit OAuth(QObject *parent = 0);
    void setApiKey(const QString consumerKey, const QString consumerSecret);
    QString consumerKey();
    QString consumerSecret();
    QString accessToken();
    QString accessTokenSecret();
    bool isAuthorized();
    QUrl authorizeUrl();

signals:
    void authorizeSuccessed();

public slots:
    void authorizePin(QString pinCode);

private:
    const QString REQUEST_TOKEN_URL = QString("https://api.twitter.com/oauth/request_token");
    const QString AUTHORIZE_URL     = QString("https://api.twitter.com/oauth/authorize");
    const QString ACCESS_TOKEN_URL  = QString("https://api.twitter.com/oauth/access_token");

    const QString DEFAULT_CONSUMER_KEY    = QString("1cIrbCiqwd3iRV5EZVHiHnzA0");
    const QString DEFAULT_CONSUMER_SECRET = QString("VYTYLtKC1p8bNUiakIEMiAvQTRSXsvRo7nBv7Srh5hvmmX7Pf9");

    QString    consumer_key;
    QString    consumer_secret;
    QByteArray request_token;
    QByteArray request_token_secret;
    QString    access_token;
    QString    access_token_secret;

    bool authorized = false;
};

#endif // OAUTH_H
