/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <meshManipulationPlugin.h>
#include <meshManipulationToolBox.h>

meshManipulationPlugin::meshManipulationPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool meshManipulationPlugin::initialize()
{
    if (!meshManipulationToolBox::registered())
    {
        qWarning() << "Unable to register meshManipulation toolbox";
    }
    return true;
}

QString meshManipulationPlugin::name() const
{
    return "Mesh Manipulation";
}

QString meshManipulationPlugin::description() const
{
    return tr("Plugin allowing to apply linear transformations.");
}

QString meshManipulationPlugin::version() const
{
    return MESHMANIPULATIONPLUGIN_VERSION;
}

QStringList meshManipulationPlugin::types() const
{
    return QStringList() << "medMeshTools";
}
