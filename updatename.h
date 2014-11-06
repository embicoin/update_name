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
        RecieveResultFailed = 0,
        NameUpdateFailed,
        Aborted,
        Executed,
        NameUpdated,
        ResultRecieved,
    };

    QString lastErrorMessage();

signals:
    void stateChanged(UpdateName::State state);
    void updateNameExecuted(QString screen_name);
    void nameUpdated(QString newName);

public slots:
    void exec(const QByteArray twitterStatusObjectJsonData);

private:
    Twitter twitter;
    QString myScreenName;
    QString errorMessage;
};

#endif // UPDATENAME_H
