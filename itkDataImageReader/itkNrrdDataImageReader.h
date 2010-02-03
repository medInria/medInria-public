#ifndef ITKNRRDDATAIMAGEREADER_H
#define ITKNRRDDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkNrrdDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT
	
public:
    itkNrrdDataImageReader(void);
    virtual ~itkNrrdDataImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNrrdDataImageReader(void);

#endif
