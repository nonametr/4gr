#include "config.h"
#include "backend.h"
#include <QDir>
#include <QMetaProperty>
#include <QSettings>
#include <QtDebug>
#include <map>
#include <vector>

const map<uint8_t, FieldData> Config::field_data = {
    { ConfigField::VERSION, { "version", "1.0" } },
    { ConfigField::DEFAULT_URL, { "default_url", "https://ru.4game.com/lineage2classic/play/" } },
    { ConfigField::TARGET_WINDOW_NAME, { "target_window_name", "Lineage II" } },
    { ConfigField::START_AFTER_PROCESS, { "start_after", "Frost" } },
    { ConfigField::AUTO_START, { "auto_start", false } },
    { ConfigField::KEEP_ALIVE, { "keep_alive", false } },
    { ConfigField::SHOW_NAVIGATION, { "show_navigation", true } },
    { ConfigField::SHOW_SSL_ERRORS, { "show_ssl_errors", false } },
    { ConfigField::SHOW_IMAGES, { "show_images", false } },
    { ConfigField::START_POS, { "start_pos", QVector2D(20, 500) } },
    { ConfigField::DELAY, { "delay", 0.0f } },
};

const map<uint8_t, FieldData> ProfileConfig::field_data = {
    { ProfileConfigField::NAME, { "name", "profile" } },
    { ProfileConfigField::URL, { "url", "https://ru.4game.com/lineage2classic/play/" } },
    { ProfileConfigField::WINDOW_POS, { "window_pos", QVector4D(10, 40, 640, 480) } },
    { ProfileConfigField::SCROLL_POS, { "scroll_pos", QVector2D(0, 0) } },
    { ProfileConfigField::TARGET_POS, { "target_pos", QVector2D(760, 380) } },
    { ProfileConfigField::ENABLED, { "enabled", true } }
};

void ProfileConfig::dataChanged(uint8_t field_id, QVariant new_val)
{
    data[field_id] = new_val;
    _owner->save(CONFIG_PATH);

    switch (field_id)
    {
    case NAME:
        return nameChanged();
    case WINDOW_POS:
        return windowPosChanged();
    case SCROLL_POS:
        return scrollPosChanged();
    case TARGET_POS:
        return targetPosChanged();
    case URL:
        return urlChanged();
    case ENABLED:
        return enabledChanged();
    default:
        break;
    }
}

void Config::dataChanged(uint8_t field_id, QVariant new_val)
{
    data[field_id] = new_val;
    save(CONFIG_PATH);

    switch (field_id)
    {
    case VERSION:
        break;
    case START_POS:
        return startPosChanged();
    case DEFAULT_URL:
        return defaultUrlChanged();
    case TARGET_WINDOW_NAME:
        return targetWindowNameChanged();
    case AUTO_START:
        return autoStartChanged();
    case KEEP_ALIVE:
        return keepAliveChanged();
    case SHOW_NAVIGATION:
        return showNavChanged();
    case SHOW_SSL_ERRORS:
        return showSSLErrorsChanged();
    case SHOW_IMAGES:
        return showImagesChanged();
    case DELAY:
        return delayChanged();
    case START_AFTER_PROCESS:
        return startAfterProcessChanged();
    default:
        break;
    }
}

void Config::_checkPath(const QString& path, bool last_try)
{
    QFileInfo check_file(path);
    if (!check_file.exists() || !check_file.isFile())
    {
        if (last_try == false)
        {
            resetConfig(path);
            return _checkPath(path, true);
        }
        else
        {
            qDebug() << "Can't write default configuration file! Please check file access permisions and restart application!";
            qTerminate();
        }
    }

    QSettings settings(path, QSettings::IniFormat);
    for (const auto& it_val : Config::field_data)
    {
        if (!settings.contains(it_val.second.name))
        {
            resetConfig(path);
            return _checkPath(path, true);
        }
    }
    settings.beginReadArray("profile");
    settings.setArrayIndex(0);
    for (const auto& it_val : ProfileConfig::field_data)
    {
        if (!settings.contains(it_val.second.name))
        {
            resetConfig(path);
            return _checkPath(path, true);
        }
    }
    settings.endArray();
}

void Config::load(QString path)
{
    path.remove("file:///");
    _checkPath(path);
    QSettings settings(path, QSettings::IniFormat);
    for (const auto& it_val : Config::field_data)
    {
        data[it_val.first] = settings.value(it_val.second.name);
    }
    int profile_count = settings.beginReadArray("profile");
    for (int i = 0; i < profile_count; ++i)
    {
        settings.setArrayIndex(i);
        ProfileConfig* profile = new ProfileConfig(this);
        for (const auto& it_val : ProfileConfig::field_data)
        {
            profile->data[it_val.first] = settings.value(it_val.second.name);
        }
        _profiles.append(profile);
    }
    settings.endArray();
}
void Config::save(QString path)
{
    path.remove("file:///");
    QSettings settings(path, QSettings::IniFormat);
    settings.clear();
    for (const auto& it_val : data)
    {
        settings.setValue(Config::field_data.at(it_val.first).name, it_val.second);
    }
    settings.beginWriteArray("profile");
    for (int i = 0; i < _profiles.length(); ++i)
    {
        settings.setArrayIndex(i);
        ProfileConfig* profile = (ProfileConfig*)_profiles[i];
        for (const auto& it_val : profile->data)
        {
            settings.setValue(ProfileConfig::field_data.at(it_val.first).name, it_val.second);
        }
    }
    settings.endArray();
}

Q_INVOKABLE void Config::appendProfile()
{
    ProfileConfig* newProfile = new ProfileConfig(this);
    for (const auto& it_val : ProfileConfig::field_data)
    {
        newProfile->data[it_val.first] = it_val.second.default_val;
    }
    newProfile->setUrl(getDefaultUrl());
    _profiles.append(newProfile);
    save(CONFIG_PATH);
}

Q_INVOKABLE void Config::resetConfig(QString path)
{
    QSettings settings(path, QSettings::IniFormat);
    settings.clear();
    for (const auto& it_val : Config::field_data)
    {
        settings.setValue(it_val.second.name, it_val.second.default_val);
    }
    settings.beginWriteArray("profile");
    for (int i = 0; i < 1; ++i)
    {
        settings.setArrayIndex(i);
        for (const auto& it_val : ProfileConfig::field_data)
        {
            settings.setValue(it_val.second.name, it_val.second.default_val);
        }
    }
    settings.endArray();
}
