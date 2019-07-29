/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medPluginLegacy.h>
#include "polygonRoiPluginExport.h"

class POLYGONROIPLUGIN_EXPORT polygonRoiPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.polygonRoiPlugin" FILE "polygonRoiPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    polygonRoiPlugin(QObject *parent = 0);
    virtual bool initialize(void);
    
    virtual QString name(void) const;
    virtual QString description(void) const;
    virtual QString version (void) const;
    virtual QStringList types(void) const;
};

