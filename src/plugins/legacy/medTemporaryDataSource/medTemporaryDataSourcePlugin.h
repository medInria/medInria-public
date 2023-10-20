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
#include "medTemporaryDataSourcePluginExport.h"

class MEDTEMPORARYDATASOURCEPLUGIN_EXPORT medTemporaryDataSourcePlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medTemporaryDataSourcePlugin" FILE "medTemporaryDataSourcePlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medTemporaryDataSourcePlugin(QObject *parent = nullptr);
    ~medTemporaryDataSourcePlugin();
    virtual bool initialize();
    
    virtual QString name() const;
    virtual QString description() const;
    virtual QString version () const;
    virtual QStringList types() const;
};
