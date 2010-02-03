#ifndef ITKGIPLDATAIMAGEWRITER_H
#define ITKGIPLDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkGiplDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkGiplDataImageWriter(void);
    virtual ~itkGiplDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkGiplDataImageWriter(void);

#endif
