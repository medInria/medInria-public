/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medRemeshingPlugin.h>
#include <medDecimateMeshProcess.h>
#include <medRefineMeshProcess.h>
#include <medSmoothMeshProcess.h>
#include <medRemeshingToolBox.h>

medRemeshingPlugin::medRemeshingPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medRemeshingPlugin::initialize()
{
    if ( !medDecimateMeshProcess::registered() )
    {
        qWarning() << "Unable to register medDecimateMeshProcess";
    }
    if ( !medRefineMeshProcess::registered() )
    {
        qWarning() << "Unable to register medRefineMeshProcess";
    }
    if ( !medSmoothMeshProcess::registered() )
    {
        qWarning() << "Unable to register medSmoothMeshProcess";
    }
    if ( !medRemeshingToolBox::registered() )
    {
        qWarning() << "Unable to register medRemeshingToolBox";
    }
    return true;
}

QString medRemeshingPlugin::name() const
{
    return "Remeshing";
}

QString medRemeshingPlugin::description() const
{
    QString description;
    description += "<h1>RemeshingTutorial</h1>";
    description += "Modify the number of polygons of a 3D mesh. You can reduce (decimate) and increase (refine) the number of polygons of your mesh, ";
    description += "choose the exact number of polygons that you need, or smooth your mesh.";
    
    description += "<h2>Refinement</h2>";
    description += "Creates four new triangles for each triangle in the mesh using vtkButterflySubdivisionFilter vtk filter.";

    description += "<h2>Decimation</h2>";
    description += "Divides by four the number of triangles using vtkDecimatePro vtk filter. You can choose to preserve the shape of the mesh during hard decimation.";

    description += "<h2>Manual change of polygon number</h2>";
    description += "Manual modification of the number of triangles.";

    description += "<h2>Smooth</h2>";
    description += "Smooth the mesh using vtkSmoothPolyDataFilter vtk filter.";
 
    description += "<br><br>This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString medRemeshingPlugin::version() const
{
    return MEDREMESHINGPLUGIN_VERSION;
}

QStringList medRemeshingPlugin::types() const
{
    return QStringList()    << "medDecimateMeshProcess"
                            << "medRefineMeshProcess"
                            << "medSmoothMeshProcess";
}
