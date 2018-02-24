#ifndef UPDATER_H
#define UPDATER_H

#include <QEventLoop>
#include <QObject>

#define APP_NAME "4grunner"
#define APP_NAME_NEW "4grunner_new"

class QNetworkAccessManager;
class QNetworkReply;

class UpdaterBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float download_progress READ getDownloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged)
    Q_PROPERTY(QString version READ getVersion NOTIFY versionChanged)

public:
    UpdaterBackend() = default;
    virtual ~UpdaterBackend() override = default;

    Q_INVOKABLE bool checkForNewVersion();
    Q_INVOKABLE void downloadNewVersion();

    const QString& getVersion() const { return version; }

    QNetworkAccessManager* network;
    QNetworkReply* reply = nullptr;
    QEventLoop versionEventLoop;
    QEventLoop updateLoop;
    const QString version = "1.0.2";

    float getDownloadProgress() const
    {
        return _download_progress;
    }
    void setDownloadProgress(float new_val)
    {
        _download_progress = new_val;
        downloadProgressChanged();
    }
public slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();

signals:
    void versionChanged();
    void downloadProgressChanged();

private:
    float _download_progress;
};

#endif // UPDATER_H
