#pragma once

#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataReader.h>

class vtkMetaDataSet;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshReaderBase: public dtkAbstractDataReader
{
    Q_OBJECT

public:
    vtkDataMeshReaderBase();

public slots:

    virtual bool canRead        (const QStringList& paths);
    virtual bool readInformation(const QStringList& paths);
    virtual bool read           (const QStringList& paths);

    virtual void setProgress(int value);

    bool extractMetaDataFromFieldData(vtkMetaDataSet* dataSet);

    // Functions needed to be declared in child classes
    virtual bool canRead        (const QString& path){return false;}
    virtual bool readInformation(const QString& path){return false;}
    virtual bool read           (const QString& path){return false;}
};
