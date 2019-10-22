/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medRegistrationWorkspace.h>
#include <medRegistrationWorkspacePlugin.h>

medRegistrationWorkspacePlugin::medRegistrationWorkspacePlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medRegistrationWorkspacePlugin::initialize()
{
    if(!medRegistrationWorkspace::registered())
    {
        qWarning() << "Unable to register medRegistrationWorkspace type";
    }
    
    return true;
}

QString medRegistrationWorkspacePlugin::name() const
{
    return "Registration Workspace";
}

QString medRegistrationWorkspacePlugin::version() const
{
    return MEDREGISTRATIONWORKSPACEPLUGIN_VERSION;
}

QStringList medRegistrationWorkspacePlugin::types() const
{
    return QStringList() << "medRegistrationWorkspace";
}
