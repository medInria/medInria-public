/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medPluginLegacy.h>

#include <variationalSegmentationPluginExport.h>

class VARIATIONALSEGMENTATIONPLUGIN_EXPORT variationalSegmentationPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.variationalSegmentationPlugin" FILE "variationalSegmentationPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    variationalSegmentationPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version () const;
    virtual QStringList types() const;
};
