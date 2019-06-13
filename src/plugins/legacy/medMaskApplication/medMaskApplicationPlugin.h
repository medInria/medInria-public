/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPluginLegacy.h>
#include "medMaskApplicationPluginExport.h"

class MEDMASKAPPLICATIONPLUGIN_EXPORT medMaskApplicationPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medMaskApplicationPlugin" FILE "medMaskApplicationPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medMaskApplicationPlugin(QObject *parent = 0);
    virtual bool initialize();
    virtual bool uninitialize(void);

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList types() const;
};
