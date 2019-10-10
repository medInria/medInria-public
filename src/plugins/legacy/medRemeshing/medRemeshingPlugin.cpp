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
    QString description = \
            "Modify the number of polygons of a 3D mesh:<br><br> \
            - <i>Refinement</i><br> \
            Creates four new triangles for each triangle in the mesh using <a href=\"https://www.vtk.org/doc/nightly/html/classvtkButterflySubdivisionFilter.html\" style=\"color: #cc0000\" >vtkButterflySubdivisionFilter</a>.<br><br> \
            - <i>Decimation</i><br> \
            Divides by four the number of triangles using <a href=\"https://www.vtk.org/doc/nightly/html/classvtkDecimatePro.html\" style=\"color: #cc0000\" >vtkDecimatePro</a>.<br><br> \
            - <i>Manual</i> modification of the number of triangles.<br><br> \
            - <i>Smooth</i> the mesh using <a href=\"https://www.vtk.org/doc/nightly/html/classvtkSmoothPolyDataFilter.html\" style=\"color: #cc0000\" >vtkSmoothPolyDataFilter</a>. \
            <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.";

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
