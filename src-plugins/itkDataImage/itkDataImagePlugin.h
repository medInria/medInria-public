/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#pragma once

#include <dtkCore/dtkPlugin.h>

#include "itkDataImagePluginExport.h"

class itkDataImagePluginPrivate;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImagePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     itkDataImagePlugin(QObject *parent = 0);
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


