#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataTensorImageWriterPluginExport.h"
#include "itkImageIOBase.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkDataTensorImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    itkDataTensorImageWriterBase();
    virtual ~itkDataTensorImageWriterBase();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    template <class PixelType>
    bool write(const QString& path, PixelType dummyArgument);

protected:
    itk::ImageIOBase::Pointer io;
};


