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

#include "vtkDataMeshReaderPluginExport.h"

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReaderPluginPrivate;

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReaderPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
public:
     vtkDataMeshReaderPlugin(QObject *parent = 0);
    ~vtkDataMeshReaderPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     vtkDataMeshReaderPluginPrivate *d;
};


