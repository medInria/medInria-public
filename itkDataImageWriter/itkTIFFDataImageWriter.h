#ifndef ITKTIFFDATAIMAGEWRITER_H
#define ITKTIFFDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkTIFFDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkTIFFDataImageWriter(void);
    virtual ~itkTIFFDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkTIFFDataImageWriter(void);

#endif
