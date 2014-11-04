#include "updatenamemain.h"

#include <QDebug>

UpdateNameMain::UpdateNameMain(QObject *parent) :
    QObject(parent)
{
    updateName.moveToThread(updateNameThread);
    updateNameThread->start();
}

UpdateNameMain::~UpdateNameMain()
{
    updateNameThread->quit();
    updateNameThread->wait();
    updateNameThread->deleteLater();
}

void UpdateNameMain::userStreamStateChanged(UserStream::State state)
{
    switch (state) {
    case UserStream::ConnectionFailed:
        qCritical() << "UserStreamに接続できませんでした。";
        loop.quit();
        break;
    case UserStream::Connecting:
        qDebug() << "UserStreamに接続しています...";
        break;
    case UserStream::Running:
        qDebug() << "UserStreamに接続しました。";
        break;
    case UserStream::DisConnected:
        qWarning() << "UserStreamから切断されました。";
        break;
    default:
        break;
    }
}

void UpdateNameMain::updateNameStateChanged(UpdateName::State state)
{
    switch (state) {
    case UpdateName::RecieveResultFailed:
        qCritical() << "結果のツイートに失敗しました。";
        qCritical() << updateName.lastErrorMessage();
        break;
    case UpdateName::NameUpdateFailed:
        qCritical() << "nameの変更に失敗しました。";
        qCritical() << updateName.lastErrorMessage();
        break;
    case UpdateName::StartupMessageFailed:
        qCritical() << "スタートアップメッセージのツイートに失敗しました。";
        qCritical() << updateName.lastErrorMessage();
        break;
    case UpdateName::Aborted:
        qWarning() << "update_nameが拒否されました。";
        break;
    case UpdateName::Executed:
        qDebug() << "update_nameが実行されました。";
        break;
    case UpdateName::StartupMessagePosted:
        qDebug() << "スタートアップメッセージをツイートしました。";
        break;
    case UpdateName::NameUpdated:
        qDebug() << "nameが変更されました。";
        break;
    case UpdateName::ResultRecieved:
        qDebug() << "結果がツイートされました。";
        break;
    default:
        break;
    }
}

void UpdateNameMain::exec()
{
    connect(&userStream, SIGNAL(receivedData(QByteArray)), &updateName, SLOT(exec(QByteArray)));
    qRegisterMetaType<UserStream::State>("UserStream::State");
    connect(&userStream, SIGNAL(stateChanged(UserStream::State)), this, SLOT(userStreamStateChanged(UserStream::State)));
    qRegisterMetaType<UpdateName::State>("UpdateName::State");
    connect(&updateName, SIGNAL(stateChanged(UpdateName::State)), this, SLOT(updateNameStateChanged(UpdateName::State)));

    userStream.start();
    loop.exec();

    disconnect(&userStream, SIGNAL(receivedData(QByteArray)), &updateName, SLOT(exec(QByteArray)));
    disconnect(&userStream, SIGNAL(stateChanged(UserStream::State)), this, SLOT(userStreamStateChanged(UserStream::State)));
    disconnect(&updateName, SIGNAL(stateChanged(UpdateName::State)), this, SLOT(updateNameStateChanged(UpdateName::State)));

    qWarning() << "update_nameを終了しました。";
}
