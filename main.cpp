#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QEventLoop>
#include <QDebug>

#include "settings.h"
#include "authorize_update_name.h"
#include "updatenamemain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Settings settings;
    UpdateNameMain m;

    if(settings.consumerKey().isEmpty() || settings.consumerSecret().isEmpty() || settings.accessToken().isEmpty() || settings.accessTokenSecret().isEmpty()) {
        authorize();
    }

    m.exec();

    return a.exec();
}
