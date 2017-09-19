/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include <medSegmentationPluginExport.h>

/**
  Provide segmentation capabilities to medInria.

  The main plugin class is called by the app. It needs to :
   - register the segmentation configuration with the app.
 */
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT medSegmentationPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    medSegmentationPlugin(QObject *parent = 0);
    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList authors() const;
    virtual QStringList contributors() const;
};
