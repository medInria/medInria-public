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
    QString description = \
            tr("Sample data values at specified point locations on a mesh.\
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
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
