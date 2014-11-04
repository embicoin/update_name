#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

#include "userstream.h"
#include "update_name_oauth.h"

UserStream::UserStream(QObject *parent) :
    QThread(parent)
{
    isStopped = true;
}

UserStream::~UserStream()
{
    //reply->deleteLater();
}

void UserStream::run()
{
    const QByteArray signatureKey = settings.consumerSecret().toUtf8()
            .append("&")
            .append(settings.accessTokenSecret().toUtf8());

    QVariantMap oauthParams;
    QUrlQuery   oauthQuery;
    QByteArray  signatureBaseString;
    QByteArray  oauthHeader;
    QNetworkRequest request(QUrl(USERSTREAM_URL));
    QNetworkAccessManager manager;
    QByteArray response;
    QByteArray buffer;
    QNetworkReply *reply;
    QEventLoop  loop;
    bool connected = false;

    isStopped = false;

    while(!isStopped) {
        emit stateChanged(Connecting);

        oauthParams["oauth_consumer_key"]     = settings.consumerKey();
        oauthParams["oauth_token"]            = settings.accessToken();
        oauthParams["oauth_signature_method"] = OAUTH_SIGNATURE_METHOD;
        oauthParams["oauth_timestamp"]        = OAUTH_TIMESTAMP;
        oauthParams["oauth_nonce"]            = OAUTH_NONCE;
        oauthParams["oauth_version"]          = OAUTH_VERSION;

        for(int i = 0; i < oauthParams.size(); i++) {
            oauthQuery.addQueryItem(oauthParams.keys()[i], oauthParams.values()[i].toString());
        }

        signatureBaseString.append("GET&")
                .append(USERSTREAM_URL.toUtf8().toPercentEncoding())
                .append("&")
                .append(oauthQuery.toString().toUtf8().toPercentEncoding());

        oauthParams["oauth_signature"] =
                QMessageAuthenticationCode::hash(signatureBaseString,
                                                 signatureKey,
                                                 QCryptographicHash::Sha1).toBase64().toPercentEncoding();

        oauthHeader.append("OAuth ");
        for(int i = 0; i < oauthParams.size(); i++) {
            oauthHeader.append(oauthParams.keys()[i])
                    .append("=\"")
                    .append(oauthParams.values()[i].toByteArray())
                    .append("\", ");
        }
        oauthHeader.chop(2);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setRawHeader("Authorization", oauthHeader);

        reply = manager.get(request);

        connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

        while(!reply->isFinished() || !isStopped || reply->error() != QNetworkReply::NoError) {
            loop.exec();

            if(reply->error() == QNetworkReply::NoError) {
                if(!connected) {
                    emit stateChanged(Running);
                    connected = true;
                }

                response = reply->readAll();

                if(response.indexOf("\r") > 0) {
                    if(!buffer.isEmpty()) {
                        response.prepend(QString::fromUtf8(buffer).split("\r").first().toUtf8());
                        buffer.replace(QString::fromUtf8(buffer).split("\r").first().toUtf8(), "");
                    }
                    emit receivedData(response);
                } else if(response.indexOf("\r") == -1) {
                    buffer.append(response);
                }
            } else {
                emit stateChanged(ConnectionFailed);
            }
        }

        emit stateChanged(DisConnected);
        connected = false;
    }

    reply->deleteLater();
}

void UserStream::stop()
{
    isStopped = true;
}
