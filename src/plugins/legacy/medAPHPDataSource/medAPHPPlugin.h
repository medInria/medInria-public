#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medPluginLegacy.h>
#include "medAPHPPluginExport.h"

class MEDAPHPPLUGIN_EXPORT medAPHPPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medAPHPPlugin" FILE "medAPHPPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medAPHPPlugin(QObject *parent = nullptr);
    virtual bool initialize();
    
    virtual QString name() const;
    virtual QString description() const;
    virtual QString version () const;
    virtual QStringList types() const;
};
