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
    QString description = \
            tr("Define the contour of a mask by placing landmarks on a volume. \
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
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
