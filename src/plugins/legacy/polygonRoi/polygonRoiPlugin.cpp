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
    QString description = \
            tr("Segment, interpolate Regions of Interest (ROI) on your volume. \
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
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
