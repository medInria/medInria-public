#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataImageBaseExport.h"
#include <itkImageIOBase.h>

class ITKDATAIMAGEBASE_EXPORT itkDataImageWriterBase: public dtkAbstractDataWriter {
    Q_OBJECT

public:

    itkDataImageWriterBase();
    virtual ~itkDataImageWriterBase();

    virtual QStringList handled() const = 0;

    /** Override base class */
    virtual QStringList supportedFileExtensions() const;

public slots:

    bool write    (const QString& path);
    bool canWrite (const QString& path);

protected:

    itk::ImageIOBase::Pointer io;

private:

    template <unsigned DIM,typename T>
    bool write_image(const QString& path,const char* type);
};


