#ifndef ITKDATAIMAGEREADERBASE_H
#define ITKDATAIMAGEREADERBASE_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "itkDataImageBaseExport.h"
#include "itkImageIOBase.h"

class ITKDATAIMAGEBASE_EXPORT itkDataImageReaderBase:
        public dtkAbstractDataReader
{
    Q_OBJECT

public:

    itkDataImageReaderBase(void);
    virtual ~itkDataImageReaderBase(void);

    virtual QStringList handled(void) const = 0;

public slots:

    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual void readInformation (const QString& path);
    virtual void readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

protected:

    itk::ImageIOBase::Pointer io;

private:

    template <unsigned DIM,typename T>
    bool read_image(const QString& path,const char* type);
};

#endif
