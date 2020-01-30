/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsBaseScu.h>

#include <dtkCoreSupport/dtkGlobal.h>

#include <QtCore>

bool medAbstractPacsBaseScu::addQueryAttribute(int group, int elem, const char* value)
{
    Q_UNUSED(group);
    Q_UNUSED(elem);
    Q_UNUSED(value);
    
    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

void medAbstractPacsBaseScu::setQueryLevel( eQueryLevel level )
{
    Q_UNUSED(level);

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
