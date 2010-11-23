#ifndef ITKDATAIMAGEREADERBASE_H
#define ITKDATAIMAGEREADERBASE_H

#include <dtkCore/dtkAbstractDataReader.h>

#include "itkDataImageReaderPluginExport.h"
#include "itkImageIOBase.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkDataImageReaderBase : public dtkAbstractDataReader
{
    Q_OBJECT
	
public:
    itkDataImageReaderBase(void);
    virtual ~itkDataImageReaderBase(void);

    virtual QStringList handled(void) const;
    
    static QStringList s_handled (void);
	
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

	
};

#endif
