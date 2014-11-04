#ifndef UPDATENAMEMAIN_H
#define UPDATENAMEMAIN_H

#include <QObject>
#include <QThread>
#include <QEventLoop>
#include "userstream.h"
#include "updatename.h"

class UpdateNameMain : public QObject
{
    Q_OBJECT
public:
    explicit UpdateNameMain(QObject *parent = 0);
    ~UpdateNameMain();

public slots:
    void exec();

private slots:
    void userStreamStateChanged(UserStream::State state);
    void updateNameStateChanged(UpdateName::State state);

private:
    UserStream userStream;
    UpdateName updateName;
    QThread *updateNameThread = new QThread;
    QEventLoop loop;
};

#endif // UPDATENAMEMAIN_H
