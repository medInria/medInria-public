#ifndef ITKDATAIMAGEWRITERBASE_H
#define ITKDATAIMAGEWRITERBASE_H

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataImageWriterPluginExport.h"
#include "itkImageIOBase.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkDataImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    itkDataImageWriterBase(void);
    virtual ~itkDataImageWriterBase(void);

    virtual QStringList handled(void) const = 0;
    
public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

protected:
    itk::ImageIOBase::Pointer io;
    
};

#endif
