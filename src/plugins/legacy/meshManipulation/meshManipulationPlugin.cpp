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
    QString description;
    description += "<h1>Mesh Manipulation Tutorial</h1>";
    description += "Apply an interactive deformation box to a mesh or several meshes.";
    description += "<ul>";
    description += "<li>Drop one or multiple meshes in the view.</li>";
    description += "<li>Scaling: hold <right-click> on the bounding box</li>";
    description += "<li>Rotation: hold <left-click> on the sides of the bounding box</li>";
    description += "<li>Translation: hold <mouse-wheel button>, or <left-click> on the central sphere of the bounding box</li>";
    description += "</ul>";
    description += "You can export and import the current transformation matrix, cancel the current transformation, and save the transformed data.";
    return description;
}

QString meshManipulationPlugin::version() const
{
    return MESHMANIPULATIONPLUGIN_VERSION;
}

QStringList meshManipulationPlugin::types() const
{
    return QStringList() << "medMeshTools";
}
