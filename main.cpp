#include "backend.h"
#include "config.h"
#include "maskedmousearea.h"
#include "system_tray.h"
#include "updater.h"
#include <QApplication>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngine>

#define NEW_BINARY_TALE "_new.exe"
#define BINARY_TALE ".exe"

bool isNewBinary(const char* bin_str)
{
    return strstr(bin_str + strlen(bin_str) - sizeof(NEW_BINARY_TALE), NEW_BINARY_TALE) != nullptr;
}

int updateBinary(const char* from)
{
    size_t from_size = strlen(from);
    char* to = new char[from_size];
    memcpy(to, from, from_size - (sizeof(NEW_BINARY_TALE) - 1));
    memcpy(to + from_size - (sizeof(NEW_BINARY_TALE) - 1), BINARY_TALE, sizeof(BINARY_TALE));

    QFile::remove(to);
    QFile::copy(from, to);

    QProcess::startDetached(to, qApp->arguments());
    delete[] to;
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    if (isNewBinary(argv[0]))
    {
        return updateBinary(argv[0]);
    }

    QApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QApplication::setOrganizationName("mmo");
    QApplication::setOrganizationDomain("magicmess.online");
    QApplication::setApplicationName("4GR");

    QtWebEngine::initialize();

    QQmlApplicationEngine engine;
    QQmlContext* ctxt = engine.rootContext();

    qmlRegisterType<MaskedMouseArea>("MMOMaskedMouseArea", 1, 0, "MaskedMouseArea");

    SystemTray tray;
    UpdaterBackend updaterBackend;
    Backend* backend = Backend::get();

    backend->config.load(CONFIG_PATH);

    ctxt->setContextProperty("tray", &tray);
    ctxt->setContextProperty("backend", backend);
    ctxt->setContextProperty("updaterBackend", &updaterBackend);
    ctxt->setContextProperty("config", &backend->config);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
