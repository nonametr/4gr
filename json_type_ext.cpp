#include "json_type_ext.h"

QT_BEGIN_NAMESPACE

void to_json(json& j, const QVector2D& val)
{
    j = json{ { "x", val.x() }, { "y", val.y() } };
}
void from_json(const json& j, QVector2D& val)
{
    val.setX(j.at("x").get<float>());
    val.setY(j.at("y").get<float>());
}

void to_json(json& j, const QVector4D& val)
{
    j = json{ { "x", val.x() }, { "y", val.y() }, { "z", val.z() }, { "w", val.w() } };
}
void from_json(const json& j, QVector4D& val)
{
    val.setX(j.at("x").get<float>());
    val.setY(j.at("y").get<float>());
    val.setZ(j.at("z").get<float>());
    val.setW(j.at("w").get<float>());
}

void to_json(nlohmann::json& j, const QString& val)
{
    j = nlohmann::json(val.toStdString());
}
void from_json(const nlohmann::json& j, QString& val)
{
    val = QString::fromStdString(j.get<std::string>());
}

QT_END_NAMESPACE
