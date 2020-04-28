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
#include "itkINRDataImageReaderPluginExport.h"

class ITKINRDATAIMAGEREADERPLUGIN_EXPORT itkINRDataImageReaderPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkINRDataImageReaderPlugin" FILE "itkINRDataImageReaderPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkINRDataImageReaderPlugin(QObject *parent = 0);
    ~itkINRDataImageReaderPlugin();

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
