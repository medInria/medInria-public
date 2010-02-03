#ifndef ITKBMPDATAIMAGEWRITER_H
#define ITKBMPDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkBMPDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkBMPDataImageWriter(void);
    virtual ~itkBMPDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkBMPDataImageWriter(void);

#endif
