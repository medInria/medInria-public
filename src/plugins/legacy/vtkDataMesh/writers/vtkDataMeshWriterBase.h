#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataWriter.h>
#include <vtkMetaDataSet.h>

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    static const QString metaDataFieldPrefix;

    vtkDataMeshWriterBase();
    ~vtkDataMeshWriterBase() override = default;

public slots:
    bool canWrite (const QString& path) override;

    void addMetaDataAsFieldData(vtkMetaDataSet* dataSet);
    void clearMetaDataFieldData(vtkMetaDataSet* dataSet);
};
