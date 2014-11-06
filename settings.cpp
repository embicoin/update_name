#include "settings.h"

Settings::Settings(QObject *parent) :
    QSettings(FILE_NAME, QSettings::IniFormat, parent)
{
    consumer_key = value("ConsumerKey").toString();
    consumer_secret = value("ConsumerSecret").toString();
    access_token = value("AccessToken").toString();
    access_token_secret = value("AccessTokenSecret").toString();
}

Settings::~Settings()
{
    //settings->deleteLater();
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
    setValue("ConsumerKey", key);
    consumer_key = key;
}

void Settings::setConsumerSecret(QString secret)
{
    setValue("ConsumerSecret", secret);
    consumer_secret = secret;
}

void Settings::setAccessToken(QString token)
{
    setValue("AccessToken", token);
    access_token = token;
}

void Settings::setAccessTokenSecret(QString token_secret)
{
    setValue("AccessTokenSecret", token_secret);
    access_token_secret = token_secret;
}
