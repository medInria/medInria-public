#include "medBoutiquesAbstractFileHandler.h"

#include <QJsonArray>
#include <QJsonValue>

FormatAndExtension::FormatAndExtension(const QJsonArray &typeAndExtension)
{
    if(typeAndExtension.size() < 2)
    {
        return;
    }
    this->type = typeAndExtension[0].toString();
    this->extension = typeAndExtension[1].toString();
}
