#ifndef VTKDATAMESH4DWRITERBASE_H
#define VTKDATAMESH4DWRITERBASE_H

#include <dtkCore/dtkAbstractDataWriter.h>

#include "vtkDataMeshWriterPluginExport.h"
class vtkDataManagerWriter;

class VTKDATAMESHWRITERPLUGIN_EXPORT vtkDataMesh4DWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
             vtkDataMesh4DWriter(void);
    virtual ~vtkDataMesh4DWriter(void);

    virtual QStringList handled(void) const;
    static  QStringList s_handled (void);

    
    virtual QString description(void) const;
	
    static bool registered(void);
    
public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

 protected:
    vtkDataManagerWriter* writer;
};


dtkAbstractDataWriter *createVtkDataMesh4DWriter(void);

#endif
