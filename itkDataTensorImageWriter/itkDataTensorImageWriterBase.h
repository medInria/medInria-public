#ifndef ITKDATATENSORIMAGEWRITERBASE_H
#define ITKDATATENSORIMAGEWRITERBASE_H

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataTensorImageWriterPluginExport.h"
#include "itkImageIOBase.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkDataTensorImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    itkDataTensorImageWriterBase(void);
    virtual ~itkDataTensorImageWriterBase(void);

    virtual QStringList handled(void) const;

    static QStringList s_handled (void);

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    template <class PixelType>
    bool write(const QString& path, PixelType dummyArgument);

protected:
    itk::ImageIOBase::Pointer io;
};

#endif // ITKDATATENSORIMAGEWRITERBASE_H
