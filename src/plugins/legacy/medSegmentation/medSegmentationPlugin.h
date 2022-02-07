#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkPlugin.h>

#include <medPluginLegacy.h>
#include <medSegmentationPluginExport.h>

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medSegmentationPluginPrivate;

/**
  Provide segmentation capabilities.

  The main plugin class is called by the app. It needs to :
   - register the segmentation configuration with the app.
 */
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medSegmentationPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medSegmentationPlugin" FILE "medSegmentationPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medSegmentationPlugin(QObject *parent = nullptr);

    virtual bool initialize();
    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;

private:
     medSegmentationPluginPrivate *d;
};
