/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"
#include "vtkDataMeshWriterBase.h"

class vtkDataSetWriter;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshWriter : public vtkDataMeshWriterBase
{
    Q_OBJECT

public:
    vtkDataMeshWriter();
    ~vtkDataMeshWriter() override = default;

    QStringList handled() const override;
    static  QStringList s_handled();

    QString description() const override;
    QString identifier() const override;

    QStringList supportedFileExtensions() const override;

    static bool registered();

public slots:
    bool write (const QString& path);

private:
    static const char ID[];
};

dtkAbstractDataWriter *createVtkDataMeshWriter();
