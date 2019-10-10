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
    QString description = \
            tr("Convert a mask to a closed surface mesh.\
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
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
