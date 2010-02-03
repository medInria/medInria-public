#ifndef ITKNIFTIDATAIMAGEREADER_H
#define ITKNIFTIDATAIMAGEREADER_H

#include "itkDataImageReaderBase.h"

#include "itkDataImageReaderPluginExport.h"

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkNiftiDataImageReader : public itkDataImageReaderBase
{
    Q_OBJECT
	
public:
    itkNiftiDataImageReader(void);
    virtual ~itkNiftiDataImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNiftiDataImageReader(void);

#endif
