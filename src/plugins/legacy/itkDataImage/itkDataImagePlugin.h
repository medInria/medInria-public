#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkPlugin.h>

#include <itkDataImagePluginExport.h>

class itkDataImagePluginPrivate;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImagePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkDataImagePlugin" FILE "itkDataImagePlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkDataImagePlugin(QObject *parent = nullptr);
    ~itkDataImagePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
    itkDataImagePluginPrivate *d;
};
