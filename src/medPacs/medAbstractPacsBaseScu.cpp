/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractPacsBaseScu.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsBaseScu::medAbstractPacsBaseScu(void)
{

}

medAbstractPacsBaseScu::~medAbstractPacsBaseScu(void)
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

void medAbstractPacsBaseScu::clearAllQueryAttributes(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

QVector<medAbstractPacsNode*> medAbstractPacsBaseScu::getNodeContainer( void )
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsNode*>();
}


