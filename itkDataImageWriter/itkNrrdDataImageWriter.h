#ifndef ITKNRRDDATAIMAGEWRITER_H
#define ITKNRRDDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkNrrdDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkNrrdDataImageWriter(void);
    virtual ~itkNrrdDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkNrrdDataImageWriter(void);

#endif
