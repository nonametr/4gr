#ifndef GLOBAL_H
#define GLOBAL_H

#include <QMetaEnum>
#include <string>

template <typename QEnum>
std::string enumToString(const QEnum value)
{
    std::string result(QMetaEnum::fromType<QEnum>().valueToKey(value));
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

#endif // GLOBAL_H
