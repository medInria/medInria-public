/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#pragma once

#include <dtkCore/dtkPlugin.h>

#include "msegPluginExport.h"

namespace mseg {
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT PluginPrivate;

/**
  Provide segmentation capabilities to medInria.

  The main plugin class is called by the app. It needs to :
   - register the segmentation configuration with the app.
 */
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT Plugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    Plugin(QObject *parent = 0);
    ~Plugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QString version() const;

    virtual QStringList authors() const;
    virtual QString contact() const;
    virtual QStringList contributors() const;
    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     PluginPrivate *d;
};
} // namespace mseg


