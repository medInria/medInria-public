/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iterativeClosestPointProcess.h>
#include "iterativeClosestPointPlugin.h"
#include "iterativeClosestPointToolBox.h"


bool iterativeClosestPointPlugin::initialize()
{
    if(!iterativeClosestPointProcess::registered())
    {
        dtkWarn() << "Unable to register iterativeClosestPoint type";
    }
    
    if(!iterativeClosestPointToolBox::registered())
    {
        dtkWarn() << "Unable to register iterativeClosestPoint toolbox";
    }
    
    return true;
}

QString iterativeClosestPointPlugin::name() const
{
    return "Iterative Closest Point";
}

QString iterativeClosestPointPlugin::version() const
{
    return ITERATIVECLOSESTPOINTPLUGIN_VERSION;
}

QString iterativeClosestPointPlugin::identifier() const
{
    return "iterativeClosestPoint";
}

QStringList iterativeClosestPointPlugin::types() const
{
    return QStringList() << "iterativeClosestPoint";
}

Q_EXPORT_PLUGIN2(iterativeClosestPointPlugin, iterativeClosestPointPlugin)
