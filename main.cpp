#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QEventLoop>
#include <QDir>
#include <QDebug>

#include "settings.h"
#include "authorize_update_name.h"
#include "updatenamemain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDir library_path;
    library_path.cd(QCoreApplication::applicationDirPath());
    library_path.cd("./lib");
    QCoreApplication::addLibraryPath(library_path.path());
    Settings settings;

    if(settings.consumerKey().isEmpty() || settings.consumerSecret().isEmpty() || settings.accessToken().isEmpty() || settings.accessTokenSecret().isEmpty()) {
        authorize();
    }
    UpdateNameMain m;

    m.exec();

    return a.exec();
}
