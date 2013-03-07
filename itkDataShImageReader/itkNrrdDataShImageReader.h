#ifndef ITKNRRDDATASHIMAGEREADER_H
#define ITKNRRDDATASHIMAGEREADER_H

#include "itkDataShImageReaderBase.h"

#include "itkDataShImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkNrrdDataShImageReader : public itkDataShImageReaderBase
{
    Q_OBJECT
	
public:
    itkNrrdDataShImageReader(void);
    virtual ~itkNrrdDataShImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNrrdDataShImageReader(void);

#endif
