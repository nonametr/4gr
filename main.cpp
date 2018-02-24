#include "backend.h"
#include "config.h"
#include "updater.h"

#include "maskedmousearea.h"
#include <QApplication>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngine>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QtWebEngine::initialize();
    QCoreApplication::setOrganizationName("mmo");
    QCoreApplication::setOrganizationDomain("magicmess.online");
    QCoreApplication::setApplicationName("4GRunner");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    if (QFileInfo(QCoreApplication::applicationFilePath()).fileName() == "4grunner_new.exe")
    {
        QFile::remove(qApp->applicationDirPath() + "/4grunner.exe");
        QFile::copy(qApp->applicationDirPath() + "/4grunner_new.exe", qApp->applicationDirPath() + "/4grunner.exe");

        QProcess::startDetached(qApp->applicationDirPath() + "/4grunner.exe", qApp->arguments());
        QCoreApplication::quit();
        return EXIT_SUCCESS;
    }
    else if (QFileInfo::exists(qApp->applicationDirPath() + "/4grunner_new.exe"))
    {
        QFile::remove(qApp->applicationDirPath() + "/4grunner_new.exe");
    }

    QQmlApplicationEngine engine;
    QQmlContext* ctxt = engine.rootContext();

    qmlRegisterType<MaskedMouseArea>("MMOMaskedMouseArea", 1, 0, "MaskedMouseArea");

    Backend* backend = Backend::create();
    unique_ptr<UpdaterBackend> updBackend{ new UpdaterBackend() };
    ctxt->setContextProperty("backend", backend);
    ctxt->setContextProperty("updaterBackend", updBackend.get());
    ctxt->setContextProperty("config", &backend->config);

    engine.load(QUrl(QStringLiteral("qrc:/settings.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
