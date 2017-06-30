/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationPlugin.h"
#include "medMaskApplicationToolBox.h"

bool medMaskApplicationPlugin::initialize()
{
    if(!medMaskApplication::registered())
    {
        dtkWarn() << "Unable to register medMaskApplication type";
    }
    
    if ( !medMaskApplicationToolBox::registered() )
    {
        dtkWarn() << "Unable to register medMaskApplication toolbox";
    }
    
    return true;
}

QString medMaskApplicationPlugin::name() const
{
    return "Mask Application";
}

QString medMaskApplicationPlugin::version() const
{
    return MEDMASKAPPLICATIONPLUGIN_VERSION;
}

QString medMaskApplicationPlugin::identifier() const
{
    return "medMaskApplication";
}

QStringList medMaskApplicationPlugin::types() const
{
    return QStringList() << "medMaskApplication";
}

Q_EXPORT_PLUGIN2(medMaskApplicationPlugin, medMaskApplicationPlugin)
