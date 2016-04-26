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
#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// reformatPluginPrivate
// /////////////////////////////////////////////////////////////////

class reformatPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * reformatPluginPrivate::s_Name = "reformat";

// /////////////////////////////////////////////////////////////////
// reformatPlugin
// /////////////////////////////////////////////////////////////////

reformatPlugin::reformatPlugin(QObject *parent) : dtkPlugin(parent), d(new reformatPluginPrivate)
{
    
}

reformatPlugin::~reformatPlugin()
{
    delete d;
    
    d = NULL;
}

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

bool reformatPlugin::uninitialize()
{
    return true;
}

QString reformatPlugin::name() const
{
    return "reformatPlugin";
}

QString reformatPlugin::description() const
{
    return tr("");
}

QString reformatPlugin::version() const
{
    return REFORMATPLUGIN_VERSION;
}

QString reformatPlugin::contact() const
{
    return QString::fromUtf8("Florent Collot <florent.collot@ihu-liryc.fr>");
}

QStringList reformatPlugin::authors() const
{
    return QStringList() << "Mathilde Merle <mathilde.merle@ihu-liryc.fr>"
                         << "Florent Collot <florent.collot@ihu-liryc.fr>";
}

QStringList reformatPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString reformatPlugin::identifier() const
{
    return reformatPluginPrivate::s_Name;
}


QStringList reformatPlugin::tags() const
{
    return QStringList();
}

QStringList reformatPlugin::types() const
{
    return QStringList() << "reformat";
}
QStringList reformatPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(reformatPlugin, reformatPlugin)
