/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "cliSupportWorkspace.h"
#include "cliSupportWorkspacePlugin.h"
#include "cliSupportToolBox.h"

#include <medWorkspaceFactory.h>
#include <medToolBoxFactory.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// cliSupportWorkspacePluginPrivate
// /////////////////////////////////////////////////////////////////

class cliSupportWorkspacePluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * cliSupportWorkspacePluginPrivate::s_Name = "cliSupportWorkspace";

// /////////////////////////////////////////////////////////////////
// cliSupportWorkspacePlugin
// /////////////////////////////////////////////////////////////////

cliSupportWorkspacePlugin::cliSupportWorkspacePlugin(QObject *parent) : dtkPlugin(parent), d(new cliSupportWorkspacePluginPrivate)
{
    
}

cliSupportWorkspacePlugin::~cliSupportWorkspacePlugin(void)
{
    delete d;
    d = NULL;
}

bool cliSupportWorkspacePlugin::initialize(void)
{
    medWorkspaceFactory * workspaceFactory = medWorkspaceFactory::instance();
    if ( ! workspaceFactory->registerWorkspace<cliSupportWorkspace>()) {
        dtkWarn() << "Unable to register cliSupportWorkspace";
    }


    medToolBoxFactory * factory = medToolBoxFactory::instance();
    if ( ! factory->registerToolBox<cliSupportToolBox>()) {
        dtkWarn() << "Unable to register cliSupportToolBox type";
    }
    
    return true;
}

bool cliSupportWorkspacePlugin::uninitialize(void)
{
    return true;
}

QString cliSupportWorkspacePlugin::name(void) const
{
    return "cliSupportWorkspacePlugin";
}

QString cliSupportWorkspacePlugin::description(void) const
{
    return tr("CommandLine Plugins Support");
}

QString cliSupportWorkspacePlugin::version(void) const
{
    return CLISUPPORTWORKSPACEPLUGIN_VERSION;
}

QString cliSupportWorkspacePlugin::contact(void) const
{
    return "";
}

QStringList cliSupportWorkspacePlugin::authors(void) const
{
    QStringList list;
    return list;
}

QStringList cliSupportWorkspacePlugin::contributors(void) const
{
    QStringList list;
    return list;
}

QString cliSupportWorkspacePlugin::identifier(void) const
{
    return cliSupportWorkspacePluginPrivate::s_Name;
}


QStringList cliSupportWorkspacePlugin::tags(void) const
{
    return QStringList();
}

QStringList cliSupportWorkspacePlugin::types(void) const
{
    return QStringList() << "cliSupportWorkspace";
}
QStringList cliSupportWorkspacePlugin::dependencies(void) const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(cliSupportWorkspacePlugin, cliSupportWorkspacePlugin)
