#ifndef USERSTREAM_H
#define USERSTREAM_H

#include <QThread>
#include <QNetworkReply>
#include <QEventLoop>

#include "settings.h"

class UserStream : public QThread
{
    Q_OBJECT
public:
    explicit UserStream(QObject *parent = 0);
    ~UserStream();

    enum State {
        ConnectionFailed = -1,
        Running          = 0,
        Connecting       = 1,
        DisConnected     = 2
    };

signals:
    void stateChanged(UserStream::State state);

signals:
    void receivedData(QByteArray data);

public slots:
    void stop();

protected:
    virtual void run();

private:
    const QString USERSTREAM_URL = "https://userstream.twitter.com/1.1/user.json";

    Settings settings;
    bool isStopped;
};

#endif // USERSTREAM_H
