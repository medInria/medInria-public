#ifndef ITKGIPLDATAIMAGEREADER_H
#define ITKGIPLDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkGiplDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT
	
public:
    itkGiplDataImageReader(void);
    virtual ~itkGiplDataImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkGiplDataImageReader(void);

#endif
