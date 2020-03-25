/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorLogEuclideanPluginPrivate;

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorLogEuclideanPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
    
public:
    itkProcessTensorLogEuclideanPlugin(QObject *parent = 0);
    ~itkProcessTensorLogEuclideanPlugin();
    
    virtual bool initialize();
    virtual bool uninitialize();
    
    virtual QString name() const;
    virtual QString identifier() const;
    virtual QString description() const;
    virtual QString contact() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;
    
    virtual QStringList tags() const;
    virtual QStringList types() const;
    
private:
    itkProcessTensorLogEuclideanPluginPrivate *d;
};


