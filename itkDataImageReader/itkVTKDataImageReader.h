#ifndef ITKVTKDATAIMAGEREADER_H
#define ITKVTKDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkVTKDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT
	
public:
    itkVTKDataImageReader(void);
    virtual ~itkVTKDataImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkVTKDataImageReader(void);

#endif
