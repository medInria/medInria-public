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

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointPluginPrivate
// /////////////////////////////////////////////////////////////////

class iterativeClosestPointPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * iterativeClosestPointPluginPrivate::s_Name = "iterativeClosestPoint";

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointPlugin
// /////////////////////////////////////////////////////////////////

iterativeClosestPointPlugin::iterativeClosestPointPlugin(QObject *parent) : dtkPlugin(parent), d(new iterativeClosestPointPluginPrivate)
{
    
}

iterativeClosestPointPlugin::~iterativeClosestPointPlugin()
{
    delete d;
    
    d = NULL;
}

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

bool iterativeClosestPointPlugin::uninitialize()
{
    return true;
}

QString iterativeClosestPointPlugin::name() const
{
    return "iterativeClosestPointPlugin";
}

QString iterativeClosestPointPlugin::description() const
{
    return tr("");
}

QString iterativeClosestPointPlugin::version() const
{
    return ITERATIVECLOSESTPOINTPLUGIN_VERSION;
}

QString iterativeClosestPointPlugin::contact() const
{
    return "";
}

QStringList iterativeClosestPointPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList iterativeClosestPointPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString iterativeClosestPointPlugin::identifier() const
{
    return iterativeClosestPointPluginPrivate::s_Name;
}


QStringList iterativeClosestPointPlugin::tags() const
{
    return QStringList();
}

QStringList iterativeClosestPointPlugin::types() const
{
    return QStringList() << "iterativeClosestPoint";
}
QStringList iterativeClosestPointPlugin::dependencies() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(iterativeClosestPointPlugin, iterativeClosestPointPlugin)
