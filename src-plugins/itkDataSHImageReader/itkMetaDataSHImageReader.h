#ifndef ITKMETADATASHIMAGEREADER_H
#define ITKMETADATASHIMAGEREADER_H

#include "itkDataSHImageReaderBase.h"

#include "itkDataSHImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkMetaDataSHImageReader : public itkDataSHImageReaderBase
{
    Q_OBJECT
	
public:
    itkMetaDataSHImageReader(void);
    virtual ~itkMetaDataSHImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static bool registered(void);	
};

dtkAbstractDataReader *createItkMetaDataSHImageReader(void);

#endif
