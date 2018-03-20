#include "config.h"
#include "backend.h"
#include "profile_config.h"
#include <QDir>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSettings>
#include <QtDebug>
#include <map>
#include <vector>

void Config::fieldChanged(ConfigField field_id)
{
    switch (field_id)
    {
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

json Config::getDefaultJson()
{
    json result;
    for (int i = 0; i < CONFIG_FIELD_MAX; ++i)
    {
        ConfigField field_id = static_cast<ConfigField>(i);
        switch (field_id)
        {
        case START_POS:
            result[enumToString(field_id)] = "{ \"x\":0.0, \"y\":0.0 }"_json;
            break;
        case DEFAULT_URL:
            result[enumToString(field_id)] = "google.com";
            break;
        case TARGET_WINDOW_NAME:
            result[enumToString(field_id)] = "Lineage II";
            break;
        case START_AFTER_PROCESS:
            result[enumToString(field_id)] = "Frost";
            break;
        case AUTO_START:
            result[enumToString(field_id)] = false;
            break;
        case KEEP_ALIVE:
            result[enumToString(field_id)] = false;
            break;
        case SHOW_NAVIGATION:
            result[enumToString(field_id)] = true;
            break;
        case SHOW_SSL_ERRORS:
            result[enumToString(field_id)] = false;
            break;
        case SHOW_IMAGES:
            result[enumToString(field_id)] = true;
            break;
        case DELAY:
            result[enumToString(field_id)] = 0.0f;
            break;
        case AUTO_SAVE:
            result[enumToString(field_id)] = false;
            break;
        default:
            qFatal("Error! Field not supported!");
            break;
        }
    }
    return result;
}
void Config::loadFromJson(string data_str)
{
    json j_profiles;
    try
    {
        j_data = json::parse(data_str);
        for (int i = 0; i < CONFIG_FIELD_MAX; ++i)
        {
            j_data.at(enumToString(static_cast<ConfigField>(i)));
        }
    }
    catch (exception e)
    {
        qDebug() << e.what();
        qDebug() << "Using default config...";
        j_data = getDefaultJson();
    }

    if (j_data.find("profiles") != j_data.end())
    {
        for (auto it = j_profiles.begin(); it != j_profiles.end(); ++it)
        {
            _profiles.append(new ProfileConfig(this, it.key()));
        }
    }
}
void Config::load(QString path)
{
    path.remove("file:///");
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Can't read config file!";
        j_data = getDefaultJson();
        return;
    }

    string file_data = file.readAll().toStdString();
    file.close();

    loadFromJson(file_data);
}
void Config::save(QString path)
{
    path.remove("file:///");

    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Can't write config file!";
        return;
    }
    file.write(j_data.dump(4).c_str());
    file.close();
}

Q_INVOKABLE void Config::appendProfile()
{
    json& j_profiles = j_data["profiles"];
    size_t profiles_count = j_profiles.size();

    string new_profile_name = "profile_" + std::to_string(profiles_count + 1);
    ProfileConfig::getDefaultJson(j_profiles[new_profile_name], new_profile_name);

    ProfileConfig* newProfile = new ProfileConfig(this, new_profile_name);
    _profiles.append(newProfile);
}
