#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QMessageAuthenticationCode>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "twitter.h"
#include "update_name_oauth.h"

Twitter::Twitter(QObject *parent) :
    QObject(parent)
{
}

/**
 * @brief Twitter::requestTwitterApi
 * @param httpMethod HTTPメソッド
 * @param url        TwitterAPIのURL
 * @param dataParams データのパラメータ
 * @return           Twitterから返ってきたデータ
 */
QByteArray Twitter::requestTwitterApi(const QNetworkAccessManager::Operation httpMethod, const QString url, const QVariantMap dataParams)
{
    const QByteArray signatureKey = settings.consumerSecret().toUtf8()
            .append("&")
            .append(settings.accessTokenSecret().toUtf8());

    QByteArray httpMethodString;
    QVariantMap oauthParams;
    QVariantMap signatureParams;
    QUrlQuery signatureQuery;
    QByteArray signatureBaseString;
    QByteArray oauthHeader;
    QUrlQuery dataQuery;
    QUrl requestUrl(url);
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QTimer timer;
    QEventLoop loop;

    switch (httpMethod) {
    case QNetworkAccessManager::GetOperation:
        httpMethodString = "GET";
        break;
    case QNetworkAccessManager::PostOperation:
        httpMethodString = "POST";
        break;
    default:
        return NULL;
        break;
    }

    oauthParams["oauth_consumer_key"]     = settings.consumerKey();
    oauthParams["oauth_token"]            = settings.accessToken();
    oauthParams["oauth_signature_method"] = OAUTH_SIGNATURE_METHOD;
    oauthParams["oauth_timestamp"]        = OAUTH_TIMESTAMP;
    oauthParams["oauth_nonce"]            = OAUTH_NONCE;
    oauthParams["oauth_version"]          = OAUTH_VERSION;

    for(int i = 0; i < oauthParams.size(); i++) {
        signatureParams.insert(oauthParams.keys()[i], oauthParams.values()[i]);
    }

    if(!dataParams.isEmpty()) {
        for(int i = 0; i < dataParams.size(); i++) {
            signatureParams.insert(dataParams.keys()[i], dataParams.values()[i]);
            dataQuery.addQueryItem(dataParams.keys()[i], dataParams.values()[i].toString());
        }
    }

    for(int i = 0; i < signatureParams.size(); i++) {
        signatureQuery.addQueryItem(signatureParams.keys()[i], signatureParams.values()[i].toString());
    }

    signatureBaseString.append(httpMethodString)
            .append("&")
            .append(url.toUtf8().toPercentEncoding())
            .append("&")
            .append(signatureQuery.toString(QUrl::FullyEncoded).toUtf8().toPercentEncoding());

    oauthParams["oauth_signature"] = QMessageAuthenticationCode::hash(signatureBaseString,
                                                                      signatureKey,
                                                                      QCryptographicHash::Sha1).toBase64().toPercentEncoding();

    oauthHeader.append("OAuth ");
    for(int i = 0; i < oauthParams.size(); i++) {
        oauthHeader.append(oauthParams.keys()[i])
                   .append("=\"")
                   .append(oauthParams.values()[i].toByteArray())
                   .append("\", ");
    }
    oauthHeader.remove(oauthHeader.length() - 2, 2);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", oauthHeader);

    switch (httpMethod) {
    case QNetworkAccessManager::GetOperation:
        if(!dataQuery.isEmpty()) {
            requestUrl.setQuery(dataQuery);
        }
        request.setUrl(requestUrl);
        reply = manager.get(request);
        break;
    case QNetworkAccessManager::PostOperation:
        request.setUrl(requestUrl);
        reply = manager.post(request, dataQuery.toString(QUrl::FullyEncoded).toUtf8());
        break;
    default:
        return NULL;
        break;
    }

    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    timer.start(15000);
    loop.exec();

    if(timer.isActive()) {
        if(reply->error() == QNetworkReply::NoError) {
            reply->deleteLater();
            return reply->readAll();
        } else {
            reply->deleteLater();
            throw std::runtime_error(QJsonDocument::fromJson(reply->readAll()).object().value("errors").toArray().first().toObject().value("message").toString().toStdString());
        }
    } else {
        reply->deleteLater();
        throw std::runtime_error(std::string("タイムアウトしました。"));
    }
    return NULL;
}

QString Twitter::getScreenName()
{
    QVariantMap dataParams;
    QByteArray response;
    try {
        response = requestTwitterApi(QNetworkAccessManager::GetOperation, ACCOUNT_VERIFY_CREDENTIALS_URL, dataParams);
    } catch (...) {
        throw;
        return NULL;
    }
    return QJsonDocument::fromJson(response).object().value("screen_name").toString();
}

QString Twitter::getName()
{
    QVariantMap dataParams;
    QByteArray response;
    try {
        response = requestTwitterApi(QNetworkAccessManager::GetOperation, ACCOUNT_VERIFY_CREDENTIALS_URL, dataParams);
    } catch(...) {
        throw;
        return NULL;
    }
    return QJsonDocument::fromJson(response).object().value("name").toString();
}

void Twitter::statusUpdate(QString text, QString in_reply_to_status_id)
{
    QVariantMap dataParams;
    dataParams["status"] = text;
    if(!in_reply_to_status_id.isEmpty()) {
        dataParams["in_reply_to_status_id"] = in_reply_to_status_id;
    }
    try {
        requestTwitterApi(QNetworkAccessManager::PostOperation, STATUSES_UPDATE_URL, dataParams);
    } catch(...) {
        throw;
    }
}

void Twitter::updateName(QString name)
{
    QVariantMap dataParams;
    dataParams["name"] = name;
    try {
        requestTwitterApi(QNetworkAccessManager::PostOperation, ACCOUNT_UPDATE_PROFILE_URL, dataParams);
    } catch(...) {
        throw;
    }
}
