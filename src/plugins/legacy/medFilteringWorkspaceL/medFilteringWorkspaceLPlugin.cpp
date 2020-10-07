/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medFilteringWorkspaceL.h>
#include <medFilteringWorkspaceLPlugin.h>

medFilteringWorkspaceLPlugin::medFilteringWorkspaceLPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medFilteringWorkspaceLPlugin::initialize()
{
    if(!medFilteringWorkspaceL::registered())
    {
        qWarning() << "Unable to register medFilteringWorkspaceL type";
    }
    
    return true;
}

QString medFilteringWorkspaceLPlugin::name() const
{
    return "Filtering Workspace";
}

QString medFilteringWorkspaceLPlugin::version() const
{
    return MEDFILTERINGWORKSPACELPLUGIN_VERSION;
}

QStringList medFilteringWorkspaceLPlugin::types() const
{
    return QStringList() << "medFilteringWorkspaceL";
}
