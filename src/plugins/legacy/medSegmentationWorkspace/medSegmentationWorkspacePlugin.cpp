/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSegmentationWorkspace.h>
#include <medSegmentationWorkspacePlugin.h>

medSegmentationWorkspacePlugin::medSegmentationWorkspacePlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medSegmentationWorkspacePlugin::initialize()
{
    if(!medSegmentationWorkspace::registered())
    {
        qWarning() << "Unable to register medSegmentationWorkspace type";
    }
    
    return true;
}

QString medSegmentationWorkspacePlugin::name() const
{
    return "Segmentation Workspace";
}

QString medSegmentationWorkspacePlugin::version() const
{
    return MEDSEGMENTATIONWORKSPACEPLUGIN_VERSION;
}

QStringList medSegmentationWorkspacePlugin::types() const
{
    return QStringList() << "medSegmentationWorkspace";
}
