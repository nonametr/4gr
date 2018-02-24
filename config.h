#ifndef CONFIG_H
#define CONFIG_H

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVector2D>
#include <QVector4D>
#include <map>
#include <mutex>
#include <string>

using namespace std;

#define CONFIG_PATH "default.conf"

enum ConfigField : uint8_t
{
    VERSION,
    START_POS,
    DEFAULT_URL,
    TARGET_WINDOW_NAME,
    AUTO_START,
    KEEP_ALIVE,
    SHOW_NAVIGATION,
    SHOW_SSL_ERRORS,
    SHOW_IMAGES,
    DELAY
};

enum ProfileConfigField : uint8_t
{
    NAME,
    WINDOW_POS,
    SCROLL_POS,
    TARGET_POS,
    URL,
    ENABLED
};

struct FieldData
{
    QString name;
    QVariant default_val;
};

class Config;

class ProfileConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool is_running READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString url READ getUrl WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QVector4D win_pos READ getWindowPos WRITE setWindowPos NOTIFY windowPosChanged)
    Q_PROPERTY(QVector2D scroll_pos READ getScrollPos WRITE setScrollPos NOTIFY scrollPosChanged)
    Q_PROPERTY(QVector2D target_pos READ getTargetPos WRITE setTargetPos NOTIFY targetPosChanged)

public:
    ProfileConfig(Config* owner) : _owner(owner)
    {
    }
    virtual ~ProfileConfig() override = default;

    QString getName() const
    {
        return data.at(ProfileConfigField::NAME).value<QString>();
    }
    QVector4D getWindowPos() const
    {
        return data.at(ProfileConfigField::WINDOW_POS).value<QVector4D>();
    }
    QVector2D getScrollPos() const
    {
        return data.at(ProfileConfigField::SCROLL_POS).value<QVector2D>();
    }
    QVector2D getTargetPos() const
    {
        return data.at(ProfileConfigField::TARGET_POS).value<QVector2D>();
    }
    QString getUrl() const
    {
        return data.at(ProfileConfigField::URL).value<QString>();
    }
    bool getEnabled() const
    {
        return data.at(ProfileConfigField::ENABLED).value<bool>();
    }
    bool isRunning() const
    {
        return _isRunning;
    }

    void setEnabled(bool new_val)
    {
        dataChanged(ProfileConfigField::ENABLED, new_val);
    }
    void setName(const QString& new_val)
    {
        dataChanged(ProfileConfigField::NAME, new_val);
    }
    void setWindowPos(const QVector4D& new_val)
    {
        dataChanged(ProfileConfigField::WINDOW_POS, new_val);
    }
    void setScrollPos(const QVector2D& new_val)
    {
        dataChanged(ProfileConfigField::SCROLL_POS, new_val);
    }
    void setTargetPos(const QVector2D& new_val)
    {
        dataChanged(ProfileConfigField::TARGET_POS, new_val);
    }
    void setUrl(const QString& new_val)
    {
        dataChanged(ProfileConfigField::URL, new_val);
    }
    void setIsRunning(bool new_val)
    {
        _isRunning = new_val;
        emit isRunningChanged();
    }

    void dataChanged(uint8_t field_id, QVariant new_val);

    bool _isRunning;
    map<uint8_t, QVariant> data;
    static const map<uint8_t, FieldData> field_data;

signals:
    void profileIdChanged();
    void nameChanged();
    void windowPosChanged();
    void scrollPosChanged();
    void targetPosChanged();
    void urlChanged();
    void enabledChanged();
    void isRunningChanged();

private:
    Config* _owner;
};

class Config : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float delay READ getDelay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(bool show_ssl_errors READ getShowSSLErrors WRITE setShowSSLErrors NOTIFY showSSLErrorsChanged)
    Q_PROPERTY(bool show_images READ getShowImages WRITE setShowImages NOTIFY showImagesChanged)
    Q_PROPERTY(bool show_nav READ getShowNav WRITE setShowNav NOTIFY showNavChanged)
    Q_PROPERTY(bool autostart READ getAutoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(bool keep_alive READ getKeepAlive WRITE setKeepAlive NOTIFY keepAliveChanged)
    Q_PROPERTY(QString default_url READ getDefaultUrl WRITE setDefaultUrl NOTIFY defaultUrlChanged)
    Q_PROPERTY(QString window_name READ getTargetWindowName WRITE setTargetWindowName NOTIFY targetWindowNameChanged)
    Q_PROPERTY(QList<QObject*> profiles READ getProfiles NOTIFY profilesChanged)
    Q_PROPERTY(QVector2D start_pos READ getStartPos WRITE setStartPos NOTIFY startPosChanged)

public:
    explicit Config(QObject* parent = 0) : QObject(parent)
    {
    }
    virtual ~Config() = default;
    Q_INVOKABLE void appendProfile();
    Q_INVOKABLE void removeProfile(int profile_id)
    {
        _profiles.removeAt(profile_id);
    }

    Q_INVOKABLE void resetConfig(QString path = CONFIG_PATH);

    Q_INVOKABLE void load(QString path);
    Q_INVOKABLE void save(QString path);

public:
    void _checkPath(const QString& path, bool last_try = false);

    QList<QObject*> getProfiles() const { return _profiles; }
    QString getTargetWindowName() const { return data.at(ConfigField::TARGET_WINDOW_NAME).value<QString>(); }
    QString getDefaultUrl() const { return data.at(ConfigField::DEFAULT_URL).value<QString>(); }
    bool getKeepAlive() const { return data.at(ConfigField::KEEP_ALIVE).value<bool>(); }
    bool getAutoStart() const { return data.at(ConfigField::AUTO_START).value<bool>(); }
    bool getShowNav() const { return data.at(ConfigField::SHOW_NAVIGATION).value<bool>(); }
    bool getShowSSLErrors() const { return data.at(ConfigField::SHOW_SSL_ERRORS).value<bool>(); }
    bool getShowImages() const { return data.at(ConfigField::SHOW_IMAGES).value<bool>(); }
    QVector2D getStartPos() const { return data.at(ConfigField::START_POS).value<QVector2D>(); }
    float getDelay() const { return data.at(ConfigField::DELAY).value<float>(); }

    void setShowImages(bool new_val)
    {
        dataChanged(ConfigField::SHOW_IMAGES, new_val);
    }
    void setShowSSLErrors(bool new_val)
    {
        dataChanged(ConfigField::SHOW_SSL_ERRORS, new_val);
    }
    void setShowNav(bool new_val)
    {
        dataChanged(ConfigField::SHOW_NAVIGATION, new_val);
    }
    void setKeepAlive(bool new_val)
    {
        dataChanged(ConfigField::KEEP_ALIVE, new_val);
    }
    void setAutoStart(bool new_val)
    {
        dataChanged(ConfigField::AUTO_START, new_val);
    }
    void setTargetWindowName(const QString& new_val)
    {
        dataChanged(ConfigField::TARGET_WINDOW_NAME, new_val);
    }
    void setDefaultUrl(const QString& new_val)
    {
        dataChanged(ConfigField::DEFAULT_URL, new_val);
    }
    void setStartPos(const QVector2D& new_val)
    {
        dataChanged(ConfigField::START_POS, new_val);
    }
    void setDelay(float new_val)
    {
        dataChanged(ConfigField::DELAY, new_val);
    }

    void dataChanged(uint8_t field_id, QVariant new_val);

    map<uint8_t, QVariant> data;
    static const map<uint8_t, FieldData> field_data;

signals:
    void profilesChanged();
    void targetWindowNameChanged();
    void defaultUrlChanged();
    void autoStartChanged();
    void keepAliveChanged();
    void showNavChanged();
    void showSSLErrorsChanged();
    void showImagesChanged();
    void startPosChanged();
    void delayChanged();

private:
    QList<QObject*> _profiles;
};
#endif // CONFIG_H
