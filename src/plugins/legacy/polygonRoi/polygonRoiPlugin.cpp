/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiPlugin.h"
#include "polygonRoiToolBox.h"
#include <defaultLabelToolBox.h>
#include <medContours.h>
#include <medContoursReader.h>
#include <medContoursWriters.h>
#include <urologyLabelToolBox.h>

polygonRoiPlugin::polygonRoiPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool polygonRoiPlugin::initialize()
{
     if(!polygonRoiToolBox::registered())
     {
         qDebug() << "Unable to register polygonRoiToolBox";
     }
     if(!defaultLabelToolBox::registered())
     {
         qDebug() << "Unable to register defaultLabelToolBox";
     }
     if(!urologyLabelToolBox::registered())
     {
         qDebug() << "Unable to register urologyLabelToolBox";
     }
     if (!medContours::registered())
     {
         qDebug() << "Unable to register medContours type";
     }
     if (!medContoursReader::registered())
     {
         qDebug() << "Unable to register medContoursReader type";
     }
     if (!medContoursWriter::registered())
     {
         qDebug() << "Unable to register medContoursWriter type";
     }
     return true;
}

QString polygonRoiPlugin::name() const
{
    return "Polygon ROI";
}

QString polygonRoiPlugin::description() const
{
    QString description;
    description += "<h1>Polygon ROI Tutorial</h1>";
    description += "Enter a data in the view and click on 'Activate Toolbox'.";

    description += "<h2>Create your first contours</h2>";
    description += "To segment a volume using polygons, click on the volume to draw your nodes. ";
    description += "Close the contour by clicking on the first node.<br>";
    description += "Switch to a new slice and you can draw a new contour.";

    description += "<h2>Correct a contour</h2>";
    description += "<h3>Repulsor</h3>";
    description += "You can adjust your segmentation with the Repulsor tool. It allows you to push your ";
    description += "nodes to fit a circle cursor which appears when you click on the volume.";

    description += "<h3>Remove node/contour/label</h3>";
    description += "To remove a component of the segmentation, put the cursor over it and use 'backspace', ";
    description += "or right-lick and choose 'Remove' in the menu.";

    description += "<h2>Interpolation</h2>";
    description += "Use 'Interpolate' to automatically interpolate contours through several slices of your ";
    description += "volume. You have a polygonROI segmentation that covers approximately the body ";
    description += "part you want to segment, but 'approximately' is not enough. You don't want to erase ";
    description += "all the segmentation (since it's better than nothing) but you want to refine it. You can ";
    description += "re-interpolate without removing undesired ROIs beforehand, because ";
    description += "interpolation is only done between master ROIs. A master ROI is: a new polygon, or ";
    description += "a modified polygon (repulsed or manually modified with handles).";

    description += "<h2>Orientations</h2>";
    description += "Clicking on one of the 3 orientation buttons split the view in 2 and display the data in ";
    description += "a new orientation, allowing you to check your segmentation from an other point of view.";

    description += "<h2>Labels</h2>";
    description += "To define a new label, click on the '+' button in the Label list.<br>";
    description += "You can rename it double-clicking on its name in the list, or right-click on a node and change the name.<br>";
    description += "You can change the current label putting the cursor on a node and right-click and choose the 'Change label' menu.";

    description += "<h2>Copy-Paste</h2>";
    description += "To copy a segmentation in the current slice, use CTRL/CMD + C or put the cursor ";
    description += "on a node then right-click and choose menu 'Copy'.<br>";
    description += "Paste it on an other slice using CTRL/CMD + V";

    description += "<h2>Save</h2>";
    description += "Click on 'Mask(s)' or 'Contour(s)' to save your segmentation. ";
    description += "You can also right-click on a node and choose the 'Save' menu.";

    description += "<br><br>This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString polygonRoiPlugin::version() const
{
    return POLYGONROIPLUGIN_VERSION;
}

QStringList polygonRoiPlugin::types() const
{
    return QStringList() << "polygonRoi" << "medContours";
}
