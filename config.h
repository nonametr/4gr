#ifndef CONFIG_H
#define CONFIG_H

#include "global.h"
#include "json.h"
#include "json_type_ext.h"
#include <QAbstractListModel>
#include <QMetaEnum>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVector2D>
#include <QVector4D>
#include <map>
#include <mutex>
#include <string>

using namespace std;
using namespace nlohmann;

#define CONFIG_PATH "default.conf"

class Config : public QObject
{
    friend class ProfileConfig;

    Q_OBJECT
    Q_PROPERTY(QList<QObject*> profiles READ getProfiles NOTIFY profilesChanged)
    Q_PROPERTY(float delay READ getDelay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(bool show_ssl_errors READ getShowSSLErrors WRITE setShowSSLErrors NOTIFY showSSLErrorsChanged)
    Q_PROPERTY(bool show_images READ getShowImages WRITE setShowImages NOTIFY showImagesChanged)
    Q_PROPERTY(bool show_nav READ getShowNav WRITE setShowNav NOTIFY showNavChanged)
    Q_PROPERTY(bool autostart READ getAutoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(bool keep_alive READ getKeepAlive WRITE setKeepAlive NOTIFY keepAliveChanged)
    Q_PROPERTY(bool auto_save READ getAutoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(QString default_url READ getDefaultUrl WRITE setDefaultUrl NOTIFY defaultUrlChanged)
    Q_PROPERTY(QString start_after_process READ getStartAfterProcess WRITE setStartAfterProcess NOTIFY startAfterProcessChanged)
    Q_PROPERTY(QString window_name READ getTargetWindowName WRITE setTargetWindowName NOTIFY targetWindowNameChanged)
    Q_PROPERTY(QVector2D start_pos READ getStartPos WRITE setStartPos NOTIFY startPosChanged)

public:
    Config() = default;
    virtual ~Config() = default;

    Q_INVOKABLE void appendProfile();
    Q_INVOKABLE void removeProfile(int profile_id) { _profiles.removeAt(profile_id); }
    Q_INVOKABLE void moveProfile(int from, int to) {}

    Q_INVOKABLE void load(QString path);
    Q_INVOKABLE void save(QString path);

    void loadFromJson(string data_str);

    json getDefaultJson();

    enum ConfigField : uint8_t
    {
        START_POS,
        DEFAULT_URL,
        TARGET_WINDOW_NAME,
        START_AFTER_PROCESS,
        AUTO_START,
        KEEP_ALIVE,
        SHOW_NAVIGATION,
        SHOW_SSL_ERRORS,
        SHOW_IMAGES,
        DELAY,
        AUTO_SAVE,
        CONFIG_FIELD_MAX
    };
    Q_ENUM(ConfigField)

    template <class T>
    T get(ConfigField field_id) const
    {
        return j_data.at(enumToString(field_id)).get<T>();
    }
    template <class T>
    void set(ConfigField field_id, T new_val)
    {
        j_data.at(enumToString(field_id)) = new_val;
        fieldChanged(field_id);
    }

    QList<QObject*> getProfiles() const { return _profiles; }
    QString getTargetWindowName() const { return get<QString>(ConfigField::TARGET_WINDOW_NAME); }
    QString getStartAfterProcess() const { return get<QString>(ConfigField::START_AFTER_PROCESS); }
    QString getDefaultUrl() const { return get<QString>(ConfigField::DEFAULT_URL); }
    bool getKeepAlive() const { return get<bool>(ConfigField::KEEP_ALIVE); }
    bool getAutoStart() const { return get<bool>(ConfigField::AUTO_START); }
    bool getShowNav() const { return get<bool>(ConfigField::SHOW_NAVIGATION); }
    bool getShowSSLErrors() const { return get<bool>(ConfigField::SHOW_SSL_ERRORS); }
    bool getShowImages() const { return get<bool>(ConfigField::SHOW_IMAGES); }
    QVector2D getStartPos() const { return get<QVector2D>(ConfigField::START_POS); }
    float getDelay() const { return get<float>(ConfigField::DELAY); }
    bool getAutoSave() const { return get<bool>(ConfigField::AUTO_SAVE); }

    void setShowImages(bool new_val) { set(ConfigField::SHOW_IMAGES, new_val); }
    void setShowSSLErrors(bool new_val) { set(ConfigField::SHOW_SSL_ERRORS, new_val); }
    void setShowNav(bool new_val) { set(ConfigField::SHOW_NAVIGATION, new_val); }
    void setKeepAlive(bool new_val) { set(ConfigField::KEEP_ALIVE, new_val); }
    void setAutoStart(bool new_val) { set(ConfigField::AUTO_START, new_val); }
    void setTargetWindowName(const QString& new_val) { set(ConfigField::TARGET_WINDOW_NAME, new_val); }
    void setStartAfterProcess(const QString& new_val) { set(ConfigField::START_AFTER_PROCESS, new_val); }
    void setDefaultUrl(const QString& new_val) { set(ConfigField::DEFAULT_URL, new_val); }
    void setStartPos(const QVector2D& new_val) { set(ConfigField::START_POS, new_val); }
    void setDelay(float new_val) { set(ConfigField::DELAY, new_val); }
    void setAutoSave(bool new_val) { set(ConfigField::AUTO_SAVE, new_val); }

    void fieldChanged(ConfigField field_id);

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
    void startAfterProcessChanged();
    void autoSaveChanged();

protected:
    json j_data;
    QList<QObject*> _profiles;
};
#endif // CONFIG_H
