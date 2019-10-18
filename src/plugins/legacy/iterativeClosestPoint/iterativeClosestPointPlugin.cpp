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
    QString description = \
            tr("Match the orientation/grid between two meshes through the \
               <a href=\"https://www.vtk.org/doc/nightly/html/classvtkIterativeClosestPointTransform.html\" style=\"color: #cc0000\" >vtkIterativeClosestPointTransform</a> filter. \
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
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
