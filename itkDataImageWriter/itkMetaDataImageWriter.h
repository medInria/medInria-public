#ifndef ITKMETADATAIMAGEWRITER_H
#define ITKMETADATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkMetaDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkMetaDataImageWriter(void);
    virtual ~itkMetaDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkMetaDataImageWriter(void);

#endif
