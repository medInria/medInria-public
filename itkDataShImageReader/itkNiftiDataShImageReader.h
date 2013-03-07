#ifndef ITKNIFTIDATASHIMAGEREADER_H
#define ITKNIFTIDATASHIMAGEREADER_H

#include "itkDataShImageReaderBase.h"

#include "itkDataShImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkNiftiDataShImageReader : public itkDataShImageReaderBase
{
    Q_OBJECT
	
public:
    itkNiftiDataShImageReader(void);
    virtual ~itkNiftiDataShImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNiftiDataShImageReader(void);

#endif
