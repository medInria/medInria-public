#ifndef ITKNIFTIDATASHIMAGEREADER_H
#define ITKNIFTIDATASHIMAGEREADER_H

#include "itkDataSHImageReaderBase.h"

#include "itkDataSHImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkNiftiDataSHImageReader : public itkDataSHImageReaderBase
{
    Q_OBJECT
	
public:
    itkNiftiDataSHImageReader(void);
    virtual ~itkNiftiDataSHImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNiftiDataSHImageReader(void);

#endif
