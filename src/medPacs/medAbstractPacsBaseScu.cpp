#include "medAbstractPacsBaseScu.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsBaseScu::medAbstractPacsBaseScu()
{

}

medAbstractPacsBaseScu::~medAbstractPacsBaseScu()
{

}
 

bool medAbstractPacsBaseScu::addQueryAttribute(int group, int elem, const char* value)
{
    DTK_UNUSED(group);
    DTK_UNUSED(elem);
    DTK_UNUSED(value);
    
    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

void medAbstractPacsBaseScu::setQueryLevel( eQueryLevel level )
{
    DTK_UNUSED(level);

    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractPacsBaseScu::clearAllQueryAttributes()
{
    DTK_DEFAULT_IMPLEMENTATION;
}

QVector<medAbstractPacsNode*> medAbstractPacsBaseScu::getNodeContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsNode*>();
}


