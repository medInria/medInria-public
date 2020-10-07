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
    QString description = \
            tr("Segmentation using manual painting. \
               <br><br>This plugin uses the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.");
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
