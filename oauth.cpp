#include "oauth.h"
#include "update_name_oauth.h"

#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

OAuth::OAuth(QObject *parent) :
    QObject(parent)
{
}

void OAuth::setApiKey(const QString consumerKey, const QString consumerSecret)
{
    if(!(consumerKey.isEmpty() || consumerSecret.isEmpty())) {
        consumer_key = consumerKey;
        consumer_secret = consumerSecret;
    }
}

QString OAuth::consumerKey()
{
    if(consumer_key.isEmpty()) {
        return DEFAULT_CONSUMER_KEY;
    } else {
        return consumer_key;
    }
}

QString OAuth::consumerSecret()
{
    if(consumer_secret.isEmpty()) {
        return DEFAULT_CONSUMER_SECRET;
    } else {
        return consumer_secret;
    }
}

QString OAuth::accessToken()
{
    if(authorized) {
        return access_token;
    } else {
        return NULL;
    }
}

QString OAuth::accessTokenSecret()
{
    if(authorized) {
        return access_token_secret;
    } else {
        return NULL;
    }
}

bool OAuth::isAuthorized()
{
    return authorized;
}

QUrl OAuth::authorizeUrl()
{
    const QByteArray signatureKey = consumerSecret().toUtf8().append("&");

    QVariantMap params;
    QByteArray signatureBaseString;
    QUrlQuery query;
    QUrl requestUrl(REQUEST_TOKEN_URL);
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QTimer timer;
    QEventLoop loop;
    QUrlQuery tokenQuery;

    authorized = false;

    params["oauth_consumer_key"]     = consumerKey();
    params["oauth_nonce"]            = OAUTH_NONCE;
    params["oauth_signature_method"] = OAUTH_SIGNATURE_METHOD;
    params["oauth_timestamp"]        = OAUTH_TIMESTAMP;
    params["oauth_version"]          = OAUTH_VERSION;

    for(int i = 0; i < params.size(); i++) {
        query.addQueryItem(params.keys()[i], params.values()[i].toString());
    }

    signatureBaseString = QByteArray("GET&")
            .append(REQUEST_TOKEN_URL.toUtf8().toPercentEncoding())
            .append("&")
            .append(query.toString().toUtf8().toPercentEncoding());

    query.addQueryItem("oauth_signature",
                       QString::fromUtf8(QMessageAuthenticationCode::hash(signatureBaseString,
                                                                          signatureKey,
                                                                          QCryptographicHash::Sha1).toBase64().toPercentEncoding()));


    requestUrl.setQuery(query);
    request = QNetworkRequest(requestUrl);
    reply = manager.get(request);

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    timer.start(15000);
    loop.exec();

    if(timer.isActive() && reply->error() == QNetworkReply::NoError) {
            tokenQuery.setQuery(QString::fromUtf8(reply->readAll()));
            request_token        = tokenQuery.queryItemValue("oauth_token").toUtf8();
            request_token_secret = tokenQuery.queryItemValue("oauth_token_secret").toUtf8();
            reply->deleteLater();
            return QUrl(AUTHORIZE_URL + QString("?oauth_token=") + QString::fromUtf8(request_token));
    } else {
        reply->deleteLater();
        return QUrl(NULL);
    }
}

void OAuth::authorizePin(QString pinCode)
{
    if(request_token.isEmpty() || request_token_secret.isEmpty()) {
        return;
    }

    const QByteArray signatureKey = consumerSecret().toUtf8().append("&").append(request_token_secret);

    QVariantMap params;
    QByteArray signatureBaseString;
    QUrlQuery query;
    QNetworkRequest request;
    QUrl requestUrl(ACCESS_TOKEN_URL);
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QTimer timer;
    QEventLoop loop;
    QUrlQuery tokenQuery;

    params["oauth_consumer_key"]     = consumerKey();
    params["oauth_nonce"]            = OAUTH_NONCE;
    params["oauth_signature_method"] = OAUTH_SIGNATURE_METHOD;
    params["oauth_timestamp"]        = OAUTH_TIMESTAMP;
    params["oauth_version"]          = OAUTH_VERSION;
    params["oauth_token"]            = request_token;
    params["oauth_verifier"]         = pinCode;

    for(int i = 0; i < params.size(); i++) {
        query.addQueryItem(params.keys()[i], params.values()[i].toString());
    }
    signatureBaseString.append("GET&").append(ACCESS_TOKEN_URL.toUtf8().toPercentEncoding()).append(query.toString(QUrl::FullyEncoded));

    query.addQueryItem("oauth_signature",
                       QMessageAuthenticationCode::hash(signatureBaseString,
                                                        signatureKey,
                                                        QCryptographicHash::Sha1).toBase64().toPercentEncoding());

    requestUrl.setQuery(query);
    request = QNetworkRequest(requestUrl);
    reply = manager.get(request);

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    timer.start(15000);
    loop.exec();

    if(timer.isActive() && reply->error() == QNetworkReply::NoError) {
            tokenQuery.setQuery(QString::fromUtf8(reply->readAll()));
            access_token        = tokenQuery.queryItemValue("oauth_token");
            access_token_secret = tokenQuery.queryItemValue("oauth_token_secret");
            authorized = true;
            emit authorizeSuccessed();
    }

    reply->deleteLater();
}
