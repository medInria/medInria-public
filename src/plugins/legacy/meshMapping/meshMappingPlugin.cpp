/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <meshMapping.h>
#include <meshMappingPlugin.h>
#include <meshMappingToolBox.h>

meshMappingPlugin::meshMappingPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool meshMappingPlugin::initialize()
{
    if(!meshMapping::registered())
    {
        qWarning() << "Unable to register meshMapping type";
    }
    
    if ( !meshMappingToolBox::registered() )
    {
        qWarning() << "Unable to register meshMapping toolbox";
    }
    
    return true;
}

QString meshMappingPlugin::name() const
{
    return "Mesh Mapping";
}

QString meshMappingPlugin::description() const
{
    QString description;
    description += "<h1>Mesh Mapping Tutorial</h1>";
    description += "Sample data values at specified point locations on a mesh. ";
    description += "It keeps the intersection of a mesh, and a data or mesh, and set these values to the mesh.";
    description += "<ul>";
    description += "<li>Drop a mesh, and a data or a mesh in the view.</li>";
    description += "<li>Select the data or mesh (from which to obtain values) in the parameters of the toolbox.</li>";
    description += "<li>Select the mesh (whose geometry will be used to determine positions to map) in the parameters of the toolbox.</li>";
    description += "<li>When you are ready, click on 'Run'.</li>";
    description += "</ul>";
    description += "This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString meshMappingPlugin::version() const
{
    return MESHMAPPINGPLUGIN_VERSION;
}

QStringList meshMappingPlugin::types() const
{
    return QStringList() << "meshMapping";
}
