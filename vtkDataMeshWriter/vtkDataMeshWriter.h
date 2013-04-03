#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include "vtkDataMeshWriterPluginExport.h"
class vtkDataSetWriter;

class VTKDATAMESHWRITERPLUGIN_EXPORT vtkDataMeshWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
             vtkDataMeshWriter();
    virtual ~vtkDataMeshWriter();

    virtual QStringList handled() const;
    static  QStringList s_handled();


    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    static const char ID[];
};


dtkAbstractDataWriter *createVtkDataMeshWriter();


