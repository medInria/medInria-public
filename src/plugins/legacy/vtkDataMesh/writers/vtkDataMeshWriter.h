/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataWriter.h>

class vtkDataSetWriter;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
             vtkDataMeshWriter();
    virtual ~vtkDataMeshWriter();

    virtual QStringList handled() const;
    static  QStringList s_handled();


    virtual QString description() const;
    virtual QString identifier() const;

    virtual QStringList supportedFileExtensions() const;

    static bool registered();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    static const char ID[];
};


dtkAbstractDataWriter *createVtkDataMeshWriter();


