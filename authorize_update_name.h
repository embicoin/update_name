#include "oauth.h"
#include "settings.h"
#include <iostream>
#include <QDebug>

using namespace std;

void authorize()
{
    int p;
    Settings settings;
    qDebug() << "update_nameを使うにはTwitterアカウントの認証が必要です。";
    qDebug() << "1. Twitterアカウントの認証をする。";
    qDebug() <<"2. 自分でConsumerKey, ConsumerSecret, AcceessToken, AccessTokenSecretを入力する。";
    qDebug() << "3. 終了する。";
    qDebug() << "いずれかの数字を入力してください。: ";
    cin >> p;
    switch (p) {
    case 1:
    {
        OAuth oauth;
        string pin;
        qDebug() << "まずはTwitterの認証ページを開いてください。";
        qDebug() << oauth.authorizeUrl().toString();
        qDebug() << "PINコードを入力してください。: ";
        cin >> pin;
        oauth.authorizePin(QString::fromStdString(pin));
        if(oauth.accessToken().isEmpty() || oauth.accessTokenSecret().isEmpty()) {
            qDebug() << "認証できませんでした。";
            exit(1);
        } else {
            settings.setConsumerKey(oauth.consumerKey());
            settings.setConsumerSecret(oauth.consumerSecret());
            settings.setAccessToken(oauth.accessToken());
            settings.setAccessTokenSecret(oauth.accessTokenSecret());
            qDebug() << "認証が完了しました。";
        }
        return;
        break;
    }
    case 2:
    {
        string ck, cs, at, ats;
        qDebug() << "ConsumerKeyを入力してください。: ";
        cin >> ck;
        if(ck.length() == 0) {
            exit(-1);
        }
        qDebug() << "ConsumerSecretを入力してください。: ";
        cin >> cs;
        if(cs.length() == 0) {
            exit(-1);
        }
        qDebug() << "AccessTokenを入力してください。: ";
        cin >> at;
        if(at.length() == 0) {
            exit(-1);
        }
        qDebug() << "AccessTokenSecretを入力してください。: ";
        cin >> ats;
        if(ats.length() == 0) {
            exit(-1);
        }
        settings.setConsumerKey(QString::fromStdString(ck));
        settings.setConsumerSecret(QString::fromStdString(cs));
        settings.setAccessToken(QString::fromStdString(at));
        settings.setAccessTokenSecret(QString::fromStdString(ats));
        qDebug() << "入力が完了しました。";
        return;
        break;
    }
    case 3:
        exit(0);
        return;
        break;
    default:
        exit(-1);
        return;
        break;
    }
}
