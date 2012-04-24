#ifndef VTKDATAMESHWRITERBASE_H
#define VTKDATAMESHWRITERBASE_H

#include <dtkCore/dtkAbstractDataWriter.h>

#include "vtkDataMeshWriterPluginExport.h"
class vtkDataSetWriter;

class VTKDATAMESHWRITERPLUGIN_EXPORT vtkDataMeshWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
             vtkDataMeshWriter(void);
    virtual ~vtkDataMeshWriter(void);

    virtual QStringList handled(void) const;
    static  QStringList s_handled (void);


    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    static const char ID[];
};


dtkAbstractDataWriter *createVtkDataMeshWriter(void);

#endif
