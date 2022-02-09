/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationPlugin.h>

#include <medAnnotationInteractor.h>

// /////////////////////////////////////////////////////////////////
// Plugin
// /////////////////////////////////////////////////////////////////

medSegmentationPlugin::medSegmentationPlugin(QObject *parent)
    : medPluginLegacy(parent)
{

}

bool medSegmentationPlugin::initialize()
{
    if(!medAnnotationInteractor::registered())
    {
        qDebug() << "Unable to register medAnnotationInteractor";
    }

    return true;
}

QString medSegmentationPlugin::name() const
{
    return "segmentationPlugin";
}

QString medSegmentationPlugin::description() const
{
    return tr("Segmentation plugin\n<br/>"
              "Manual painting of regions: inside, and outside."
              "<br/> Exports masks with three values: inside (1), outside (2)"
              "and other (0). You may leave out the outside colour "
              "for simple binary masks.");
}

QString medSegmentationPlugin::version() const
{
    return MEDSEGMENTATIONPLUGIN_VERSION;
}

QStringList medSegmentationPlugin::authors() const
{
    QStringList list;
    list << "John Stark";
    return list;
}

QStringList medSegmentationPlugin::contributors() const
{
    QStringList list;
    list <<  QString::fromUtf8("Benoît Bleuzé")
             << "Olivier Commowick";
    return list;
}
