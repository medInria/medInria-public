#ifndef ITKDATASHIMAGEWRITERBASE_H
#define ITKDATASHIMAGEWRITERBASE_H

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataSHImageWriterPluginExport.h"
#include "itkImageIOBase.h"

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkDataSHImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
                        itkDataSHImageWriterBase(void);
    virtual ~itkDataSHImageWriterBase(void);

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

#endif // ITKDATASHIMAGEWRITERBASE_H
