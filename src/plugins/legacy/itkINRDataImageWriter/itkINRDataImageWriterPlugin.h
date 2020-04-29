#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkPlugin.h>
#include "itkINRDataImageWriterPluginExport.h"

class ITKINRDATAIMAGEWRITERPLUGIN_EXPORT itkINRDataImageWriterPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkINRDataImageWriterPlugin" FILE "itkINRDataImageWriterPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkINRDataImageWriterPlugin(QObject *parent = 0);
    ~itkINRDataImageWriterPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QString contact() const;
    virtual QStringList authors() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;
};
