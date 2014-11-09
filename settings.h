#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QObject>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    QString consumerKey();
    QString consumerSecret();
    QString accessToken();
    QString accessTokenSecret();

public slots:
    void setConsumerKey(QString key);
    void setConsumerSecret(QString secret);
    void setAccessToken(QString token);
    void setAccessTokenSecret(QString token_secret);

private:
    const QString FILE_NAME = "update_name_config.ini";

    QSettings *settings = new QSettings(FILE_NAME, QSettings::IniFormat, this);

    QString consumer_key;
    QString consumer_secret;
    QString access_token;
    QString access_token_secret;
};

#endif // SETTINGS_H
