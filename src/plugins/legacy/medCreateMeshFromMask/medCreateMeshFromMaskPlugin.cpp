/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medCreateMeshFromMask.h>
#include <medCreateMeshFromMaskPlugin.h>
#include <medCreateMeshFromMaskToolBox.h>

medCreateMeshFromMaskPlugin::medCreateMeshFromMaskPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medCreateMeshFromMaskPlugin::initialize()
{
    if( !medCreateMeshFromMask::registered() )
    {
        qWarning() << "Unable to register medCreateMeshFromMask process";
    }
    if ( !medCreateMeshFromMaskToolBox::registered() )
    {
        qWarning() << "Unable to register medCreateMeshFromMask toolbox";
    }

    return true;
}

QString medCreateMeshFromMaskPlugin::description() const
{
    QString description;
    description += "<h1>Convert a Mesh from Mask Tutorial</h1>";
    description += "Convert a mask to a closed surface mesh:";
    description += "<ul>";
    description += "<li>Drop a volume or mask in the view.</li>";
    description += "<li>If needed, choose a threshold. Pixel values over or equal to this threshold are going to be computed.</li>";
    description += "<li>You can choose to decimate the output mesh, then choose the reduction factor.</li>";
    description += "<li>You can choose to smooth the output mesh, then choose the iteration number and the relaxation factor.</li>";
    description += "<li>When you are ready, click on 'Run'.</li>";
    description += "</ul>";
    description += "This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString medCreateMeshFromMaskPlugin::name() const
{
    return "Create Mesh From Mask";
}

QString medCreateMeshFromMaskPlugin::version() const
{
    return MEDCREATEMESHFROMMASKPLUGIN_VERSION;
}

QStringList medCreateMeshFromMaskPlugin::types() const
{
    return QStringList() << "medCreateMeshFromMask";
}
