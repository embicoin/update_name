#ifndef UPDATENAME_H
#define UPDATENAME_H

#include <QObject>

#include "twitter.h"

class UpdateName : public Twitter
{
    Q_OBJECT

public:
    explicit UpdateName(QObject *parent = 0);

    enum State {
        RecieveResultFailed  = -4,
        NameUpdateFailed     = -3,
        StartupMessageFailed = -1,
        Aborted              = 0,
        StartupMessagePosted = 1,
        Executed             = 2,
        NameUpdated          = 3,
        ResultRecieved       = 4
    };

    QString lastErrorMessage();

signals:
    void stateChanged(UpdateName::State state);
    void updateNameExecuted(QString screen_name);
    void nameUpdated(QString newName);

public slots:
    void exec(const QByteArray twitterStatusObjectJsonData);

private:
    void postStartupMessage();

    Twitter twitter;
    QString myScreenName;
    QString errorMessage;
};

#endif // UPDATENAME_H
