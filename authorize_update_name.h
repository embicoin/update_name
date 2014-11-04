#include "oauth.h"
#include "settings.h"
#include <iostream>

using namespace std;

void authorize()
{
    int p;
    Settings settings;
    cout << "update_nameを使うにはTwitterアカウントの認証が必要です。" << endl;
    cout << "1. Twitterアカウントの認証をする。" << endl;
    cout << "2. 自分でConsumerKey, ConsumerSecret, AcceessToken, AccessTokenSecretを入力する。" << endl;
    cout << "3. 終了する。" << endl;
    cout << "いずれかの数字を入力してください。: ";
    cin >> p;
    switch (p) {
    case 1:
    {
        OAuth oauth;
        string pin;
        cout << "まずはTwitterの認証ページを開いてください。" << endl;
        cout << oauth.authorizeUrl().toString().toStdString() << endl;
        cout << "PINコードを入力してください。: ";
        cin >> pin;
        oauth.authorizePin(QString::fromStdString(pin));
        if(oauth.accessToken().isEmpty() || oauth.accessTokenSecret().isEmpty()) {
            cout << "認証できませんでした。" << endl;
            exit(1);
        } else {
            settings.setConsumerKey(oauth.consumerKey());
            settings.setConsumerSecret(oauth.consumerSecret());
            settings.setAccessToken(oauth.accessToken());
            settings.setAccessTokenSecret(oauth.accessTokenSecret());
            cout << "認証が完了しました。" << endl;
        }
        return;
        break;
    }
    case 2:
    {
        string ck, cs, at, ats;
        cout << "ConsumerKeyを入力してください。: ";
        cin >> ck;
        if(ck.length() == 0) {
            exit(-1);
        }
        cout << "ConsumerSecretを入力してください。: ";
        cin >> cs;
        if(cs.length() == 0) {
            exit(-1);
        }
        cout << "AccessTokenを入力してください。: ";
        cin >> at;
        if(at.length() == 0) {
            exit(-1);
        }
        cout << "AccessTokenSecretを入力してください。: ";
        cin >> ats;
        if(ats.length() == 0) {
            exit(-1);
        }
        settings.setConsumerKey(QString::fromStdString(ck));
        settings.setConsumerSecret(QString::fromStdString(cs));
        settings.setAccessToken(QString::fromStdString(at));
        settings.setAccessTokenSecret(QString::fromStdString(ats));
        cout << "入力が完了しました。" << endl;
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
