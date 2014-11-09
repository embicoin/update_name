#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QTimer>

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
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QByteArray response;
    QByteArray buffer;
    QNetworkReply *reply = new QNetworkReply;
    QEventLoop  loop;
    QTimer timer;
    bool connected = false;
    int failedCount = 0;

    isStopped = false;

    while(!isStopped && failedCount <= 5) {
        emit stateChanged(Connecting);

        oauthParams.clear();
        oauthParams["oauth_consumer_key"]     = settings.consumerKey();
        oauthParams["oauth_token"]            = settings.accessToken();
        oauthParams["oauth_signature_method"] = OAUTH_SIGNATURE_METHOD;
        oauthParams["oauth_timestamp"]        = OAUTH_TIMESTAMP;
        oauthParams["oauth_nonce"]            = OAUTH_NONCE;
        oauthParams["oauth_version"]          = OAUTH_VERSION;

        oauthQuery.clear();
        for(int i = 0; i < oauthParams.size(); i++) {
            oauthQuery.addQueryItem(oauthParams.keys()[i], oauthParams.values()[i].toString());
        }

        signatureBaseString.clear();
        signatureBaseString.append("GET&")
                .append(USERSTREAM_URL.toUtf8().toPercentEncoding())
                .append("&")
                .append(oauthQuery.toString().toUtf8().toPercentEncoding());

        oauthParams["oauth_signature"] =
                QMessageAuthenticationCode::hash(signatureBaseString,
                                                 signatureKey,
                                                 QCryptographicHash::Sha1).toBase64().toPercentEncoding();

        oauthHeader.clear();
        oauthHeader.append("OAuth ");
        for(int i = 0; i < oauthParams.size(); i++) {
            oauthHeader.append(oauthParams.keys()[i])
                    .append("=\"")
                    .append(oauthParams.values()[i].toByteArray())
                    .append("\", ");
        }
        oauthHeader.chop(2);

        request.setUrl(QUrl(USERSTREAM_URL));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setRawHeader("Authorization", oauthHeader);

        reply = manager.get(request);

        connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

        timer.setSingleShot(true);

        buffer.clear();

        while(!reply->isFinished() && !isStopped && reply->error() == QNetworkReply::NoError) {
            timer.start(60000);
            loop.exec();

            if(reply->error() == QNetworkReply::NoError && timer.isActive()) {
                if(!connected) {
                    emit stateChanged(Running);
                    connected = true;
                    failedCount = 0;
                }
                response = reply->readAll();

                //qDebug() << "response" << response;

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
                //qCritical() << reply->errorString();
                failedCount++;
                break;
            }
        }
        emit stateChanged(DisConnected);
        connected = false;

        if(failedCount <= 5) {
            qWarning() << "再接続まで5秒待機します。";
            sleep(5);
        } else {
            break;
        }
    }

    reply->deleteLater();

    emit finished();
}

void UserStream::stop()
{
    isStopped = true;
}
