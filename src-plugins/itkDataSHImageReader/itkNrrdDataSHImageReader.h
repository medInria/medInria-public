#ifndef ITKNRRDDATASHIMAGEREADER_H
#define ITKNRRDDATASHIMAGEREADER_H

#include "itkDataSHImageReaderBase.h"

#include "itkDataSHImageReaderPluginExport.h"

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkNrrdDataSHImageReader : public itkDataSHImageReaderBase
{
    Q_OBJECT
	
public:
    itkNrrdDataSHImageReader(void);
    virtual ~itkNrrdDataSHImageReader(void);
	
    virtual QString description(void) const;
    virtual QString identifier(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNrrdDataSHImageReader(void);

#endif
