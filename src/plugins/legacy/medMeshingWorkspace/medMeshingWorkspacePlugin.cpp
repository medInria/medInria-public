/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medMeshingWorkspace.h>
#include <medMeshingWorkspacePlugin.h>

medMeshingWorkspacePlugin::medMeshingWorkspacePlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medMeshingWorkspacePlugin::initialize()
{
    if(!medMeshingWorkspace::registered())
    {
        qWarning() << "Unable to register medMeshingWorkspace type";
    }
    
    return true;
}

QString medMeshingWorkspacePlugin::name() const
{
    return "Meshing Workspace";
}

QString medMeshingWorkspacePlugin::version() const
{
    return MEDMESHINGWORKSPACEPLUGIN_VERSION;
}

QStringList medMeshingWorkspacePlugin::types() const
{
    return QStringList() << "medMeshingWorkspace";
}
