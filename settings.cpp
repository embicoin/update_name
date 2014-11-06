#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    consumer_key = settings->value("ConsumerKey").toString();
    consumer_secret = settings->value("ConsumerSecret").toString();
    access_token = settings->value("AccessToken").toString();
    access_token_secret = settings->value("AccessTokenSecret").toString();
}

Settings::~Settings()
{
    settings->deleteLater();
}

QString Settings::consumerKey()
{
    return consumer_key;
}

QString Settings::consumerSecret()
{
    return consumer_secret;
}

QString Settings::accessToken()
{
    return access_token;
}

QString Settings::accessTokenSecret()
{
    return access_token_secret;
}

void Settings::setConsumerKey(QString key)
{
    settings->setValue("ConsumerKey", key);
    consumer_key = key;
}

void Settings::setConsumerSecret(QString secret)
{
    settings->setValue("ConsumerSecret", secret);
    consumer_secret = secret;
}

void Settings::setAccessToken(QString token)
{
    settings->setValue("AccessToken", token);
    access_token = token;
}

void Settings::setAccessTokenSecret(QString token_secret)
{
    settings->setValue("AccessTokenSecret", token_secret);
    access_token_secret = token_secret;
}
