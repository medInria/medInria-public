#ifndef ITKVTKDATAIMAGEWRITER_H
#define ITKVTKDATAIMAGEWRITER_H

#include "itkDataImageWriterBase.h"

#include "itkDataImageWriterPluginExport.h"

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkVTKDataImageWriter : public itkDataImageWriterBase
{
    Q_OBJECT
	
public:
    itkVTKDataImageWriter(void);
    virtual ~itkVTKDataImageWriter(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataWriter *createItkVTKDataImageWriter(void);

#endif
