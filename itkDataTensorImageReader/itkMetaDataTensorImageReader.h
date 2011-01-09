#ifndef ITKMETADATATENSORIMAGEREADER_H
#define ITKMETADATATENSORIMAGEREADER_H

#include "itkDataTensorImageReaderBase.h"

#include "itkDataTensorImageReaderPluginExport.h"

class ITKDATATENSORIMAGEREADERPLUGIN_EXPORT itkMetaDataTensorImageReader : public itkDataTensorImageReaderBase
{
    Q_OBJECT
	
public:
    itkMetaDataTensorImageReader(void);
    virtual ~itkMetaDataTensorImageReader(void);
	
    virtual QString description(void) const;
	
    static bool registered(void);	
};

dtkAbstractDataReader *createItkMetaDataTensorImageReader(void);

#endif
