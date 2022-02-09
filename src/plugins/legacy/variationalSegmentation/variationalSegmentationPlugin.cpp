/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medToolBoxFactory.h>

#include <variationalSegmentation.h>
#include <variationalSegmentationPlugin.h>
#include <varSegToolBox.h>

variationalSegmentationPlugin::variationalSegmentationPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool variationalSegmentationPlugin::initialize()
{
    if(!variationalSegmentation::registered())
    {
        qDebug() << "Unable to register variationalSegmentation type";
    }
    medToolBoxFactory *factory = medToolBoxFactory::instance();

    if(!factory->registerToolBox<mseg::VarSegToolBox>())
    {
        qDebug() << "Unable to register VarSegToolBox type";
    }

    return true;
}

QString variationalSegmentationPlugin::name() const
{
    return "Variational Segmentation";
}

QString variationalSegmentationPlugin::description() const
{
    QString description;
    description += "<h1>Variational Segmentation Tutorial</h1>";
    description += "Drop a volume in the view and click on 'Start Segmentation'.";

    description += "<h2>Segmentation</h2>";
    description += "Click on the data to add some ";
    description += "landmarks on the contour, and at least one inside or outside the area you ";
    description += "want to segment.";

    description += "<h3>Type of landmarks to set</h3>";
    description += "<ul>";
    description += "<li>Inside VOI: \n\tShift + right mouse button</li>";
    description += "<li>On VOI: \n\tShift + left mouse button</li>";
    description += "<li>Outside VOI: \n\tShift + Ctrl + right mouse button \n\tor Shift + middle mouse button</li>";
    description += "<li>To delete a landmark: \n\tCtrl + left mouse button on the landmark</li>";
    description += "</ul>";

    description += "<h2>Results</h2>";
    description += "When you are done, click on 'Apply segmentation' to see the ";
    description += "result. You can then generate your binary volume clicking on 'Save Mask'.";

    description += "<br><br>This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString variationalSegmentationPlugin::version() const
{
    return VARIATIONALSEGMENTATIONPLUGIN_VERSION;
}

QStringList variationalSegmentationPlugin::types() const
{
    return QStringList() << "variationalSegmentation";
}
