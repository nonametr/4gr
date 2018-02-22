#include "updater.h"
#include "backend.h"
#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QUrl>

bool UpdaterBackend::checkForNewVersion()
{
    network = new QNetworkAccessManager(this);
    QUrl url("http://magicmess.online/4grunner/check_version.php");
    QNetworkReply* reply = network->get(QNetworkRequest(url));

    connect(reply, SIGNAL(finished()), &versionEventLoop, SLOT(quit()));
    versionEventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QString new_version = reply->readAll();

        if (version != new_version)
        {
            return true;
        }
    }
    else
    {
        qDebug() << reply->errorString();
    }
    return false;
}

void UpdaterBackend::downloadNewVersion()
{
    QUrl url("http://magicmess.online/4grunner/4grunner.exe");
    reply = network->get(QNetworkRequest(url));

    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void UpdaterBackend::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    setDownloadProgress(((float)bytesReceived / bytesTotal) * 100);
}

void UpdaterBackend::downloadFinished()
{
    QFile* file = new QFile(qApp->applicationDirPath() + "/4grunner_new.exe");
    file->open(QIODevice::ReadWrite);
    file->setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
    file->write(reply->readAll());
    file->flush();
    delete file;

    reply->deleteLater();
    network->deleteLater();

    QProcess::startDetached(qApp->applicationDirPath() + "/4grunner_new.exe");
    QCoreApplication::quit();
}
