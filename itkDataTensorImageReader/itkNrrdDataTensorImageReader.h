#ifndef ITKNRRDDATATENSORIMAGEREADER_H
#define ITKNRRDDATATENSORIMAGEREADER_H

#include "itkDataTensorImageReaderBase.h"

#include "itkDataTensorImageReaderPluginExport.h"

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkNrrdDataTensorImageReader : public itkDataTensorImageReaderBase
{
    Q_OBJECT
	
public:
    itkNrrdDataTensorImageReader(void);
    virtual ~itkNrrdDataTensorImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkNrrdDataTensorImageReader(void);

#endif
