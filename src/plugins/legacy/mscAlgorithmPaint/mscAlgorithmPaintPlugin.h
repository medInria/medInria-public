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
#include <mscAlgorithmPaintPluginExport.h>

namespace msc
{

class MSCALGORITHMPAINT_EXPORT AlgorithmPaintPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.mscAlgorithmPaintPlugin" FILE "mscAlgorithmPaintPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    AlgorithmPaintPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList types() const;
};

}
