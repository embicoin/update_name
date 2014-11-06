#include "updatename.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

UpdateName::UpdateName(QObject *parent) :
    Twitter(parent)
{
    try {
        myScreenName = twitter.getScreenName();
    } catch(std::runtime_error &e) {
        qCritical() << "screen_nameの取得に失敗しました。: " << e.what();
    }

    try {
        twitter.statusUpdate("update_nameが起動されました。");
    } catch(std::runtime_error &e) {
        qCritical() << "スタートアップメッセージのツイートに失敗しました。: " << e.what();
    }
}

QString UpdateName::lastErrorMessage()
{
    return errorMessage;
}

void UpdateName::exec(const QByteArray twitterStatusObjectJsonData)
{
    QJsonObject statusObject        = QJsonDocument::fromJson(twitterStatusObjectJsonData).object();
    QJsonObject userObject          = statusObject.value("user").toObject();
    QString text                    = statusObject.value("text").toString();
    const QString status_id         = statusObject.value("id_str").toString();
    const QString user_screen_name  = userObject.value("screen_name").toString();
    const QRegExp updateNameRegExp1 = QRegExp(QString("^.*@").append(myScreenName).append("\\s+update_name\\s+.*"));
    const QRegExp updateNameRegExp2 = QRegExp(QString("^\\s*.+\\s*\\(@").append(myScreenName).append("\\).*$"));
    QString newName;
    QString updatedName;

    if(text.isEmpty() || text.startsWith("RT")) {
        return;
    }

    //qDebug() << text;
    //qDebug() << status_id;

    if(updateNameRegExp1.exactMatch(text)) {
        newName = text.remove(QRegExp(QString("^.*@").append(myScreenName).append("\\s+update_name\\s+")));
    } else if(updateNameRegExp2.exactMatch(text)) {
        newName = text.remove(QRegExp(QString("\\s*\\(@").append(myScreenName).append("\\).*$")));
    } else {
        return;
    }

    //qDebug() << newName;

    if(newName.isEmpty()) {
        return;
    } else if(newName.length() > 20) {
        emit stateChanged(Aborted);
        try {
            twitter.statusUpdate(QString(".@").append(user_screen_name).append(" 名前は20文字までですよ。"), status_id);
        } catch(std::runtime_error &e) {
            errorMessage = QString::fromStdString(e.what());
            emit stateChanged(RecieveResultFailed);
            return;
        }
        emit stateChanged(ResultRecieved);
        return;
    }

    try {
        twitter.updateName(newName);
    } catch(std::runtime_error &e) {
        errorMessage = QString::fromStdString(e.what());
        emit stateChanged(NameUpdateFailed);
        try {
            twitter.statusUpdate(QString(".@").append(user_screen_name).append(" update_nameに失敗しました。\n").append(QString::fromStdString(e.what())), status_id);
        } catch(std::runtime_error &e) {
            errorMessage = QString::fromStdString(e.what());
            emit stateChanged(RecieveResultFailed);
            return;
        }
        emit stateChanged(ResultRecieved);
        return;
    }

    emit stateChanged(NameUpdated);

    try {
        updatedName = twitter.getName();
    } catch(...) {
        updatedName = newName;
    }

    try {
        twitter.statusUpdate(QString(".@").append(user_screen_name).append(" nameを\"").append(updatedName).append("\"に変更しました。"), status_id);
    } catch(std::runtime_error &e) {
        emit stateChanged(RecieveResultFailed);
        errorMessage = QString::fromStdString(e.what());
        return;
    }
    emit stateChanged(ResultRecieved);
}
