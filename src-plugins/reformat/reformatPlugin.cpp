/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCropToolBox.h>
#include "medReformatWorkspace.h"
#include "reformatPlugin.h"
#include <resliceToolBox.h>
#include <resampleProcess.h>


bool reformatPlugin::initialize()
{
    if(!medReformatWorkspace::registered())
    {
        dtkWarn() << "Unable to register medReformatWorkspace type";
    }
    if (!resliceToolBox::registered())
    {
        dtkWarn() << "Unable to register resliceToolBox type";
    }
    if (!medCropToolBox::registered())
    {
        dtkWarn() << "Unable to register medCropToolBox type";
    }
    if (!resampleProcess::registered())
    {
        dtkWarn() << "Unable to register resample process";
    }
    return true;
}

QString reformatPlugin::name() const
{
    return "reformatPlugin";
}

QString reformatPlugin::version() const
{
    return REFORMATPLUGIN_VERSION;
}

QString reformatPlugin::identifier() const
{
    return "reformat";
}

QStringList reformatPlugin::types() const
{
    return QStringList() << "reformat" << "crop" << "reslice" << "resample";
}

Q_EXPORT_PLUGIN2(reformatPlugin, reformatPlugin)
