#pragma once

#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataWriter.h>
#include <vtkMetaDataSet.h>

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    static const QString metaDataFieldPrefix;

    vtkDataMeshWriterBase();

public slots:
    bool canWrite (const QString& path);

    void addMetaDataAsFieldData(vtkMetaDataSet* dataSet);
    void clearMetaDataFieldData(vtkMetaDataSet* dataSet);
};
