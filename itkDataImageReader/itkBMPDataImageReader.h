#ifndef ITKBMPDATAIMAGEREADER_H
#define ITKBMPDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkBMPDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT
	
public:
    itkBMPDataImageReader(void);
    virtual ~itkBMPDataImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkBMPDataImageReader(void);

#endif
