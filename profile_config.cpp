#include "profile_config.h"

void ProfileConfig::fieldChanged(ProfileConfigField field_id)
{
    switch (field_id)
    {
    case DISPLAY_NAME:
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

void ProfileConfig::getDefaultJson(json& j_data, const string& name)
{
    for (int i = 0; i < PROFILE_CONFIG_FIELD_MAX; ++i)
    {
        ProfileConfigField field_id = static_cast<ProfileConfigField>(i);
        switch (field_id)
        {
        case DISPLAY_NAME:
            j_data[enumToString(field_id)] = name;
            break;
        case WINDOW_POS:
            j_data[enumToString(field_id)] = "{ \"x\":0.0, \"y\":0.0, \"z\":640.0, \"w\":480.0 }"_json;
            break;
        case SCROLL_POS:
            j_data[enumToString(field_id)] = "{ \"x\":0.0, \"y\":0.0 }"_json;
            break;
        case TARGET_POS:
            j_data[enumToString(field_id)] = "{ \"x\":0.0, \"y\":0.0 }"_json;
            break;
        case URL:
            j_data[enumToString(field_id)] = "google.com";
            break;
        case ENABLED:
            j_data[enumToString(field_id)] = true;
            break;
        default:
            qFatal("Error! Field not supported!");
            break;
        }
    }
}
