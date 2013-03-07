#ifndef ITKMETADATASHIMAGEREADER_H
#define ITKMETADATASHIMAGEREADER_H

#include "itkDataShImageReaderBase.h"

#include "itkDataShImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkMetaDataShImageReader : public itkDataShImageReaderBase
{
    Q_OBJECT
	
public:
    itkMetaDataShImageReader(void);
    virtual ~itkMetaDataShImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);	
};

dtkAbstractDataReader *createItkMetaDataShImageReader(void);

#endif
