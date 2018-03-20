#ifndef JSON_TYPES_H
#define JSON_TYPES_H

#include "json.h"
#include <QString>
#include <QVector2D>
#include <QVector4D>

using namespace nlohmann;

QT_BEGIN_NAMESPACE

void to_json(json& j, const QVector2D& val);
void from_json(const json& j, QVector2D& val);

void to_json(json& j, const QVector4D& val);
void from_json(const json& j, QVector4D& val);

void to_json(json& j, const QString& val);
void from_json(const json& j, QString& val);

QT_END_NAMESPACE

#endif // JSON_TYPES_H
