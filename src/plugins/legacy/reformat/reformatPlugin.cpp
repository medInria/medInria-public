/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCropToolBox.h>
#include <medReformatWorkspace.h>

#include <reformatPlugin.h>
#include <resliceToolBox.h>
#include <resampleProcess.h>

reformatPlugin::reformatPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool reformatPlugin::initialize()
{
    if(!medReformatWorkspace::registered())
    {
        qWarning() << "Unable to register medReformatWorkspace type";
    }
    if (!resliceToolBox::registered())
    {
        qWarning() << "Unable to register resliceToolBox type";
    }
    if (!medCropToolBox::registered())
    {
        qWarning() << "Unable to register medCropToolBox type";
    }
    if (!resampleProcess::registered())
    {
        qWarning() << "Unable to register resample process";
    }
    return true;
}

QString reformatPlugin::description() const
{
    QString description;
    description += "This plugin implements two toolboxes: ";
    description += "<h2>Cropping</h2>";
    description += "Crop a volume into a smaller one. You can manually move and transform the cropping box on the view holding the edges with your mouse.<br>";
    description += "From the View settings toolbox, you can change the volume orientation and adapt the cropping box in this new direction.<br>";
    description += "When you are ready, click on 'Apply' to apply the segmentation to the volume. The output is displayed in the view.<br>";
    description += "Click on 'Save' to save the output in the database.";

    description += "<h2>Reslice</h2>";
    description += "Reorient a volume, change its spacing or dimension.";
    description += "<ul>";
    description += "<li>Drop a volume in the view.</li>";
    description += "<li>Click on 'Start Reslice'.</li>";
    description += "<li>The view is split in 3 orientations and 1 view in 3D.</li>";
    description += "<li>Hold the red and green axis with your mouse to rotate the volume.</li>";
    description += "<li>Click on 'o' to reset the orientations.</li>";
    description += "<li>You can change the volume constrast holding left-click on the views.</li>";
    description += "<li>Click on 'r' to reset the contrast changes.</li>";
    description += "<li>In the toolbox, you can choose between 'Spacing' and 'Dimension' parameters to apply to the volume.</li>";
    description += "<li>When you are ready, click on 'Save Image' to export the reformated volume in the database.</li>";
    description += "</ul>";

    description += "This plugin uses the ITK and VTK libraries: https://itk.org and https://vtk.org";
  return description;
}

QString reformatPlugin::name() const
{
    return "Reformat";
}

QString reformatPlugin::version() const
{
    return REFORMATPLUGIN_VERSION;
}

QStringList reformatPlugin::types() const
{
    return QStringList() << "medReformatWorkspace" << "medCropToolBox" << "resliceToolBox" << "resampleProcess";
}
