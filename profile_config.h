#ifndef PROFILE_H
#define PROFILE_H

#include "config.h"
#include "global.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector2D>
#include <QVector4D>
#include <map>

using namespace std;

class ProfileConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool is_running READ isRunning WRITE setIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString url READ getUrl WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QVector4D win_pos READ getWindowPos WRITE setWindowPos NOTIFY windowPosChanged)
    Q_PROPERTY(QVector2D scroll_pos READ getScrollPos WRITE setScrollPos NOTIFY scrollPosChanged)
    Q_PROPERTY(QVector2D target_pos READ getTargetPos WRITE setTargetPos NOTIFY targetPosChanged)

public:
    ProfileConfig(Config* owner, string profile_name = "") : _name(profile_name), _owner(owner) {}
    virtual ~ProfileConfig() override = default;

    static void getDefaultJson(json& j_data, const string& name);

    enum ProfileConfigField : uint8_t
    {
        DISPLAY_NAME,
        WINDOW_POS,
        SCROLL_POS,
        TARGET_POS,
        URL,
        ENABLED,
        PROFILE_CONFIG_FIELD_MAX
    };
    Q_ENUM(ProfileConfigField)

    template <class T>
    T get(ProfileConfigField field_id) const
    {
        json j_profiles = _owner->j_data.at("profiles");
        json j_profile = j_profiles.at(_name);
        return j_profile.at(enumToString(field_id)).get<T>();
        //return _owner->j_data.at("profiles").at(_name).at(enumToString(field_id)).get<T>();
    }
    template <class T>
    void set(ProfileConfigField field_id, T new_val)
    {
        _owner->j_data.at("profiles").at(_name).at(enumToString(field_id)) = new_val;
        fieldChanged(field_id);
    }

    QString getName() const { return get<QString>(ProfileConfigField::DISPLAY_NAME); }
    QVector4D getWindowPos() const { return get<QVector4D>(ProfileConfigField::WINDOW_POS); }
    QVector2D getScrollPos() const { return get<QVector2D>(ProfileConfigField::SCROLL_POS); }
    QVector2D getTargetPos() const { return get<QVector2D>(ProfileConfigField::TARGET_POS); }
    QString getUrl() const { return get<QString>(ProfileConfigField::URL); }
    bool isEnabled() const { return get<bool>(ProfileConfigField::ENABLED); }
    bool isRunning() const { return _isRunning; }

    void setEnabled(bool new_val) { set(ProfileConfigField::ENABLED, new_val); }
    void setName(const QString& new_val) { set(ProfileConfigField::DISPLAY_NAME, new_val); }
    void setWindowPos(const QVector4D& new_val) { set(ProfileConfigField::WINDOW_POS, new_val); }
    void setScrollPos(const QVector2D& new_val) { set(ProfileConfigField::SCROLL_POS, new_val); }
    void setTargetPos(const QVector2D& new_val) { set(ProfileConfigField::TARGET_POS, new_val); }
    void setUrl(const QString& new_val) { set(ProfileConfigField::URL, new_val); }
    void setIsRunning(bool new_val)
    {
        _isRunning = new_val;
        emit isRunningChanged();
    }

    void fieldChanged(ProfileConfigField field_id);

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
    string _name;
    Config* _owner;
    bool _isRunning = false;
};

#endif // PROFILE_H
