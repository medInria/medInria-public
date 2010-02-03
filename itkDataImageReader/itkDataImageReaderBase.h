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
	
public slots:
    virtual bool canRead (QString path);
    virtual void readInformation (QString path);    
    virtual bool read (QString path);
	
    virtual void setProgress (int value);
    
protected:
	itk::ImageIOBase::Pointer io;
	
};

#endif