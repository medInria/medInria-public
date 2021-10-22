/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iterativeClosestPointProcess.h>
#include "iterativeClosestPointPlugin.h"
#include "iterativeClosestPointToolBox.h"

iterativeClosestPointPlugin::iterativeClosestPointPlugin(QObject *parent)
    : medPluginLegacy(parent)
{
}

bool iterativeClosestPointPlugin::initialize()
{
    if(!iterativeClosestPointProcess::registered())
    {
        qWarning() << "Unable to register iterativeClosestPoint type";
    }
    
    if(!iterativeClosestPointToolBox::registered())
    {
        qWarning() << "Unable to register iterativeClosestPoint toolbox";
    }
    
    return true;
}

QString iterativeClosestPointPlugin::description() const
{
    QString description;
    description += "<h1>Iterative Closest Point Tutorial</h1>";
    description += "This toolbox matches the orientation and grid between two meshes.";

    description += "<h2>Data</h2>";
    description += "Drop 2 meshes in the view, then select them in the 'Select the source mesh' and 'Select the target mesh' parameters.";

    description += "<h2>Parameters</h2>";
    description += "<ul>";
    description += "<li>Matching centroids: start the process by translating source centroid to target centroid</li>";
    description += "<li>Choose your type of transformation: Rigid body, Similarity or Affine</li>";
    description += "<li>Check mean distance: force the algorithm to check the mean distance between two iterations</li>";
    description += "<li>Scale Factor: set a linear transformation according to this factor</li>";
    description += "<li>Max Mean Distance: set the maximum mean distance between two iterations</li>";
    description += "<li>Max Num Iterations: set the maximum number of iterations</li>";
    description += "<li>Max Num Landmarks: set the maximum number of landmarks</li>";
    description += "</ul>";
    description += "When you are ready, click on 'Run'.";

    description += "<br><br>This plugin uses the vtkIterativeClosestPointTransform class from VTK library: https://vtk.org";
    return description;
}

QString iterativeClosestPointPlugin::name() const
{
    return "Iterative Closest Point";
}

QString iterativeClosestPointPlugin::version() const
{
    return ITERATIVECLOSESTPOINTPLUGIN_VERSION;
}

QStringList iterativeClosestPointPlugin::types() const
{
    return QStringList() << "iterativeClosestPoint";
}
