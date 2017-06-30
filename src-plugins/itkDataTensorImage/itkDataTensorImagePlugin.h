/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include <itkDataTensorImagePluginExport.h>

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImagePluginPrivate;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImagePlugin : public medPlugin
{
    Q_OBJECT

public:
     itkDataTensorImagePlugin(QObject *parent = 0);
    ~itkDataTensorImagePlugin();

    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     itkDataTensorImagePluginPrivate *d;
};


