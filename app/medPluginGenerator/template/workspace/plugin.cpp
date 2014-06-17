/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1Workspace.h>
#include <%1Plugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// %1PluginPrivate
// /////////////////////////////////////////////////////////////////

class %1PluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * %1PluginPrivate::s_Name = "%1";

// /////////////////////////////////////////////////////////////////
// %1Plugin
// /////////////////////////////////////////////////////////////////

%1Plugin::%1Plugin(QObject *parent) : dtkPlugin(parent), d(new %1PluginPrivate)
{
    
}

%1Plugin::~%1Plugin()
{
    delete d;
    
    d = NULL;
}

bool %1Plugin::initialize()
{
    if(!%1Workspace::registered())
        dtkWarn() << "Unable to register %1 type";
    
    return true;
}

bool %1Plugin::uninitialize()
{
    return true;
}

QString %1Plugin::name() const
{
    return "%1Plugin";
}

QString %1Plugin::description() const
{
    return tr("%3");
}

QString %1Plugin::version() const
{
    return %2PLUGIN_VERSION;
}

QString %1Plugin::contact() const
{
    return "";
}

QStringList %1Plugin::authors() const
{
    QStringList list;
    return list;
}

QStringList %1Plugin::contributors() const
{
    QStringList list;
    return list;
}

QString %1Plugin::identifier() const
{
    return %1PluginPrivate::s_Name;
}


QStringList %1Plugin::tags() const
{
    return QStringList();
}

QStringList %1Plugin::types() const
{
    return QStringList() << "%1";
}
QStringList %1Plugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(%1Plugin, %1Plugin)
