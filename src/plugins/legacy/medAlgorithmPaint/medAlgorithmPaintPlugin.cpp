/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAlgorithmPaintPlugin.h>
#include <medAlgorithmPaintToolBox.h>

namespace med
{

AlgorithmPaintPlugin::AlgorithmPaintPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool AlgorithmPaintPlugin::initialize()
{
    if(!AlgorithmPaintToolBox::registered())
    {
        qDebug() << "Unable to register AlgorithmPaintToolBox";
    }
    return true;
}

QString AlgorithmPaintPlugin::name() const
{
    return "Algorithm Paint";
}

QString AlgorithmPaintPlugin::description() const
{
    QString description;
    description += "<h1>Paint Segmentation Tutorial</h1>";
    description += "Draw a brush mask on your volume in the view. You can change the value and color of the mask using the label parameter.<p>";

    description += "<h2>Paint / Erase</h2>";
    description += "A left click on the volume in the view draws a mask area under the mouse cursor, a right click removes it. ";
    description += "The mouse cursor area can be adjusted using the 'Brush Radius (mm)' slider, or <em>CTRL + mouse wheel</em>.";

    description += "<h2>Magic Wand</h2>";
    description += "The Magic Wand tool applies region growing segmentation. ";
    description += "A left click on the data sets the seed. Check the pixel value at top left of the data to ";
    description += "define your lower and upper thresholds. Enter you chosen thresholds in the toolbox. <br>";
    description += "The '3D mode' parameter applies 3D region growing across the data slices. <br>";
    description += "The 'RealTime Computation' parameter updates the mask when ";
    description += "the lower and upper thresholds or the seed change.";

    description += "<h3>How to choose your thresholds in Magic Wand</h3>";
    description += "<br><img src=\":paintTutoMagicWand.png\" width=\"380\"/>";

    description += "<br><br>This plugin uses the ITK library: https://itk.org";
    return description;
}

QString AlgorithmPaintPlugin::version() const
{
    return MEDALGORITHMPAINT_VERSION;
}

QStringList AlgorithmPaintPlugin::types() const
{
    return QStringList() << "Segmentation";
}

}
